"""
intransitive_alphazero.py

AlphaZero-ready implementation of the game "Intransitive".

Dependencies:
    numpy
    torch

Optional:
    numba  # can be added later for even faster CPU simulation

Game assumptions:
    - 9x9 board
    - 2 players
    - 4 Paper, 3 Rock, 3 Scissors per player
    - pieces move like chess kings
    - Rock captures Scissors
    - Scissors captures Paper
    - Paper captures Rock
    - Red goal: bottom-left
    - Blue goal: top-right
    - reaching the goal immediately wins
    - no legal moves = loss
    - 100 consecutive moves without a capture = draw
    - a capture resets the no-capture counter
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Optional
import math

import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F

SEED = 42

np.random.seed(SEED)
torch.manual_seed(SEED)

if torch.cuda.is_available():
    torch.cuda.manual_seed_all(SEED)


# ============================================================
# Constants
# ============================================================

BOARD_SIZE = 9
N_SQUARES = 81

RED = 0
BLUE = 1

EMPTY = 0

PAPER = 0
ROCK = 1
SCISSORS = 2

# Piece encoding
RED_PAPER = 1
RED_ROCK = 2
RED_SCISSORS = 3

BLUE_PAPER = 4
BLUE_ROCK = 5
BLUE_SCISSORS = 6

N_ACTIONS = 81 * 8  # 648

DRAW = -2
ONGOING = -1

WIN_REWARD = 1.0
LOSS_REWARD = -1.0
DRAW_REWARD = 0.0


# ============================================================
# Capture table
# ============================================================

# CAPTURES[a, d] = True iff piece type a can capture piece type d.
CAPTURES = np.zeros((3, 3), dtype=np.bool_)

CAPTURES[ROCK, SCISSORS] = True
CAPTURES[SCISSORS, PAPER] = True
CAPTURES[PAPER, ROCK] = True


# ============================================================
# Board geometry
# ============================================================

# Direction ordering is fixed forever.
DIRECTIONS = (
    (-1, -1),
    (0, -1),
    (1, -1),
    (-1, 0),
    (1, 0),
    (-1, 1),
    (0, 1),
    (1, 1),
)

# neighbors[square, direction] = destination square
# -1 means that direction goes off the board.
NEIGHBORS = np.full(
    (N_SQUARES, 8),
    -1,
    dtype=np.int16,
)

for y in range(BOARD_SIZE):
    for x in range(BOARD_SIZE):
        source = y * BOARD_SIZE + x

        for d, (dx, dy) in enumerate(DIRECTIONS):
            nx = x + dx
            ny = y + dy

            if 0 <= nx < BOARD_SIZE and 0 <= ny < BOARD_SIZE:
                NEIGHBORS[source, d] = ny * BOARD_SIZE + nx


# ============================================================
# Action encoding
# ============================================================

def encode_action(source: int, direction: int) -> int:
    """
    Action ID:
        source * 8 + direction

    Range:
        [0, 647]
    """
    return source * 8 + direction


def decode_action(action: int) -> tuple[int, int]:
    return action >> 3, action & 7


def action_destination(action: int) -> int:
    source = action >> 3
    direction = action & 7
    return int(NEIGHBORS[source, direction])


# Precompute destination for every action.
ACTION_DESTINATIONS = np.full(
    N_ACTIONS,
    -1,
    dtype=np.int16,
)

for source in range(N_SQUARES):
    for direction in range(8):
        action = source * 8 + direction
        ACTION_DESTINATIONS[action] = NEIGHBORS[source, direction]


# ============================================================
# Piece lookup tables
# ============================================================

PIECE_OWNER = np.full(7, -1, dtype=np.int8)
PIECE_TYPE = np.full(7, -1, dtype=np.int8)

for p in (RED_PAPER, RED_ROCK, RED_SCISSORS):
    PIECE_OWNER[p] = RED

for p in (BLUE_PAPER, BLUE_ROCK, BLUE_SCISSORS):
    PIECE_OWNER[p] = BLUE

PIECE_TYPE[RED_PAPER] = PAPER
PIECE_TYPE[RED_ROCK] = ROCK
PIECE_TYPE[RED_SCISSORS] = SCISSORS

PIECE_TYPE[BLUE_PAPER] = PAPER
PIECE_TYPE[BLUE_ROCK] = ROCK
PIECE_TYPE[BLUE_SCISSORS] = SCISSORS


# ============================================================
# Initial setup
# ============================================================

RED_GOAL = 0 * BOARD_SIZE + 0      # (0, 0) bottom-left
BLUE_GOAL = 8 * BOARD_SIZE + 8     # (8, 8) top-right

# ----------------------------------------------------------------
# PLACEHOLDER SETUP
# ----------------------------------------------------------------

INITIAL_POSITIONS = (
    # Red (lowercase) - second player
    (4, 7, RED_PAPER),
    (5, 6, RED_PAPER),
    (6, 5, RED_PAPER),
    (7, 4, RED_PAPER),

    (5, 7, RED_ROCK),
    (6, 6, RED_ROCK),
    (7, 5, RED_ROCK),

    (4, 6, RED_SCISSORS),
    (5, 5, RED_SCISSORS),
    (6, 4, RED_SCISSORS),

    # Blue (uppercase) - first player
    (1, 4, BLUE_PAPER),
    (2, 3, BLUE_PAPER),
    (3, 2, BLUE_PAPER),
    (4, 1, BLUE_PAPER),

    (1, 3, BLUE_ROCK),
    (2, 2, BLUE_ROCK),
    (3, 1, BLUE_ROCK),

    (2, 4, BLUE_SCISSORS),
    (3, 3, BLUE_SCISSORS),
    (4, 2, BLUE_SCISSORS),
)


# ============================================================
# Game state
# ============================================================

@dataclass(slots=True)
class State:
    """
    Compact game state.

    board:
        np.uint8 array of shape (81,)

    player:
        player whose turn it is

    winner:
        -1 = ongoing
        -2 = draw
         0 = red wins
         1 = blue wins

    move_count:
        number of moves played

    no_capture_count:
        number of consecutive moves played without a capture
    """

    board: np.ndarray
    player: int
    winner: int = ONGOING
    move_count: int = 0
    no_capture_count: int = 0

    def copy(self) -> "State":
        return State(
            self.board.copy(),
            self.player,
            self.winner,
            self.move_count,
            self.no_capture_count,
        )

    def render(self):
        symbols = {
            EMPTY: ".",
            RED_PAPER: "p",
            RED_ROCK: "r",
            RED_SCISSORS: "s",
            BLUE_PAPER: "P",
            BLUE_ROCK: "R",
            BLUE_SCISSORS: "S",
        }

        for y in range(BOARD_SIZE - 1, -1, -1):
            row = []

            for x in range(BOARD_SIZE):
                square = y * BOARD_SIZE + x

                if square == BLUE_GOAL:
                    row.append("T")
                elif square == RED_GOAL:
                    row.append("t")
                else:
                    row.append(symbols[self.board[square]])

            print(" ".join(row))


# ============================================================
# Game engine
# ============================================================

class Intransitive:

    def __init__(self, max_moves: int = 100):
        self.max_moves = max_moves

        self.initial_board = np.zeros(
            N_SQUARES,
            dtype=np.uint8,
        )

        for x, y, piece in INITIAL_POSITIONS:
            self.initial_board[y * BOARD_SIZE + x] = piece

    # --------------------------------------------------------
    # State creation
    # --------------------------------------------------------

    def initial_state(self) -> State:
        return State(
            board=self.initial_board.copy(),
            player=BLUE,
            winner=ONGOING,
            move_count=0,
            no_capture_count=0
        )

    # --------------------------------------------------------
    # Legality
    # --------------------------------------------------------

    @staticmethod
    def is_legal(
        state: State,
        action: int,
    ) -> bool:

        if state.winner != ONGOING:
            return False

        if not 0 <= action < N_ACTIONS:
            return False

        source = action >> 3
        destination = ACTION_DESTINATIONS[action]

        if destination < 0:
            return False

        attacker = int(state.board[source])

        if attacker == EMPTY:
            return False

        if PIECE_OWNER[attacker] != state.player:
            return False

        defender = int(state.board[destination])

        # Empty destination.
        if defender == EMPTY:
            return True

        # Own piece.
        if PIECE_OWNER[defender] == state.player:
            return False

        # Capture.
        attacker_type = PIECE_TYPE[attacker]
        defender_type = PIECE_TYPE[defender]

        return bool(CAPTURES[attacker_type, defender_type])

    # --------------------------------------------------------
    # Legal action generation
    # --------------------------------------------------------

    def legal_actions(self, state: State) -> np.ndarray:
        """
        Returns an array of legal action IDs.

        This is optimized for MCTS:
        only scans squares containing the current player's pieces.
        """

        board = state.board
        player = state.player

        # Maximum possible number of legal actions is 80.
        # We use a fixed temporary buffer to avoid list allocations.
        result = np.empty(80, dtype=np.int16)
        count = 0

        for source in range(N_SQUARES):

            piece = int(board[source])

            if piece == EMPTY:
                continue

            if PIECE_OWNER[piece] != player:
                continue

            for direction in range(8):

                destination = NEIGHBORS[source, direction]

                if destination < 0:
                    continue

                defender = int(board[destination])

                if defender == EMPTY:
                    result[count] = source * 8 + direction
                    count += 1
                    continue

                if PIECE_OWNER[defender] == player:
                    continue

                attacker_type = PIECE_TYPE[piece]
                defender_type = PIECE_TYPE[defender]

                if CAPTURES[attacker_type, defender_type]:
                    result[count] = source * 8 + direction
                    count += 1

        return result[:count].copy()

    # --------------------------------------------------------
    # Legal action mask
    # --------------------------------------------------------

    def legal_mask(self, state: State) -> np.ndarray:
        """
        Boolean array of shape (648,).
        """

        mask = np.zeros(
            N_ACTIONS,
            dtype=np.bool_,
        )

        actions = self.legal_actions(state)
        mask[actions] = True

        return mask

    # --------------------------------------------------------
    # Transition
    # --------------------------------------------------------

    def apply(
        self,
        state: State,
        action: int,
        validate: bool = False,
    ) -> State:

        if validate and not self.is_legal(state, action):
            raise ValueError(
                f"Illegal action: {action}"
            )

        board = state.board.copy()

        source = action >> 3
        destination = int(
            ACTION_DESTINATIONS[action]
        )

        piece = board[source]
        defender = board[destination]

        # A capture occurs iff the destination contained
        # an opponent piece.
        captured = defender != EMPTY

        board[destination] = piece
        board[source] = EMPTY

        player = state.player

        # Total number of moves played.
        new_move_count = state.move_count + 1

        # Number of consecutive moves since the last capture.
        new_no_capture_count = (
            0
            if captured
            else state.no_capture_count + 1
        )

        # --------------------------------------------------------
        # Goal condition
        # --------------------------------------------------------

        goal = (
            RED_GOAL
            if player == RED
            else BLUE_GOAL
        )

        if destination == goal:
            return State(
                board=board,
                player=1 - player,
                winner=player,
                move_count=new_move_count,
                no_capture_count=new_no_capture_count,
            )

        # --------------------------------------------------------
        # 100 consecutive non-capture moves = draw
        # --------------------------------------------------------

        if new_no_capture_count >= self.max_moves:
            return State(
                board=board,
                player=1 - player,
                winner=DRAW,
                move_count=new_move_count,
                no_capture_count=new_no_capture_count,
            )

        # --------------------------------------------------------
        # Continue game
        # --------------------------------------------------------

        next_player = 1 - player

        next_state = State(
            board=board,
            player=next_player,
            winner=ONGOING,
            move_count=new_move_count,
            no_capture_count=new_no_capture_count,
        )

        # No legal moves = next player loses.
        if len(self.legal_actions(next_state)) == 0:
            return State(
                board=board,
                player=next_player,
                winner=player,
                move_count=new_move_count,
                no_capture_count=new_no_capture_count,
            )

        return next_state

    # --------------------------------------------------------
    # Terminal
    # --------------------------------------------------------

    @staticmethod
    def terminal(state: State) -> bool:
        return state.winner != ONGOING

    # --------------------------------------------------------
    # Reward
    # --------------------------------------------------------

    @staticmethod
    def reward(
        state: State,
        perspective: int,
    ) -> float:

        if state.winner == DRAW:
            return DRAW_REWARD

        if state.winner == ONGOING:
            return 0.0

        if state.winner == perspective:
            return WIN_REWARD

        return LOSS_REWARD

    # --------------------------------------------------------
    # Neural-network observation
    # --------------------------------------------------------

    def observation(self, state: State) -> np.ndarray:
        """
        Player-relative observation.

        Shape:
            (10, 9, 9)

        Channels:
            0: current player's Paper
            1: current player's Rock
            2: current player's Scissors

            3: opponent's Paper
            4: opponent's Rock
            5: opponent's Scissors

            6: current player's goal
            7: opponent's goal

            8: no-capture progress
            9: constant plane retained for architecture compatibility

        We deliberately do NOT rotate the board.

        This means action IDs retain their global coordinate system,
        which simplifies MCTS and policy handling.
        """

        board = state.board.reshape(9, 9)

        if state.player == RED:
            mine = (
                RED_PAPER,
                RED_ROCK,
                RED_SCISSORS,
            )
            opponent = (
                BLUE_PAPER,
                BLUE_ROCK,
                BLUE_SCISSORS,
            )

            my_goal = RED_GOAL
            opponent_goal = BLUE_GOAL

        else:
            mine = (
                BLUE_PAPER,
                BLUE_ROCK,
                BLUE_SCISSORS,
            )
            opponent = (
                RED_PAPER,
                RED_ROCK,
                RED_SCISSORS,
            )

            my_goal = BLUE_GOAL
            opponent_goal = RED_GOAL

        obs = np.zeros(
            (10, 9, 9),
            dtype=np.float32,
        )

        for channel, piece in enumerate(mine):
            obs[channel] = board == piece

        for channel, piece in enumerate(opponent, start=3):
            obs[channel] = board == piece

        gy, gx = divmod(my_goal, 9)
        oy, ox = divmod(opponent_goal, 9)

        obs[6, gy, gx] = 1.0
        obs[7, oy, ox] = 1.0

        obs[8] = state.no_capture_count / self.max_moves
        obs[9] = 1.0

        return obs



# ============================================================
# Residual network
# ============================================================

class ResidualBlock(nn.Module):

    def __init__(self, channels: int):
        super().__init__()

        self.conv1 = nn.Conv2d(
            channels,
            channels,
            kernel_size=3,
            padding=1,
            bias=False,
        )

        self.norm1 = nn.GroupNorm(
            num_groups=8,
            num_channels=channels,
        )

        self.conv2 = nn.Conv2d(
            channels,
            channels,
            kernel_size=3,
            padding=1,
            bias=False,
        )

        self.norm2 = nn.GroupNorm(
            num_groups=8,
            num_channels=channels,
        )

    def forward(self, x):
        residual = x

        x = self.conv1(x)
        x = self.norm1(x)
        x = F.relu(x)

        x = self.conv2(x)
        x = self.norm2(x)

        x = x + residual

        return F.relu(x)


class PolicyValueNet(nn.Module):
    """
    Small AlphaZero-style policy/value network.

    Input:
        B x 10 x 9 x 9

    Output:
        policy_logits:
            B x 648

        value:
            B
    """

    def __init__(
        self,
        channels: int = 64,
        num_blocks: int = 5,
    ):
        super().__init__()

        self.stem = nn.Sequential(
            nn.Conv2d(
                10,
                channels,
                kernel_size=3,
                padding=1,
                bias=False,
            ),
            nn.GroupNorm(
                num_groups=8,
                num_channels=channels,
            ),
            nn.ReLU(),
        )

        self.residual = nn.Sequential(
            *[
                ResidualBlock(channels)
                for _ in range(num_blocks)
            ]
        )

        # -------------------------
        # Policy head
        # -------------------------

        self.policy_conv = nn.Conv2d(
            channels,
            32,
            kernel_size=1,
        )

        self.policy_fc = nn.Linear(
            32 * 9 * 9,
            N_ACTIONS,
        )

        # -------------------------
        # Value head
        # -------------------------

        self.value_conv = nn.Conv2d(
            channels,
            32,
            kernel_size=1,
        )

        self.value_fc1 = nn.Linear(
            32 * 9 * 9,
            128,
        )

        self.value_fc2 = nn.Linear(
            128,
            1,
        )

    def forward(self, x):

        x = self.stem(x)
        x = self.residual(x)

        # Policy
        p = self.policy_conv(x)
        p = F.relu(p)
        p = p.flatten(1)
        policy_logits = self.policy_fc(p)

        # Value
        v = self.value_conv(x)
        v = F.relu(v)
        v = v.flatten(1)
        v = F.relu(self.value_fc1(v))
        value = torch.tanh(
            self.value_fc2(v)
        ).squeeze(-1)

        return policy_logits, value


# ============================================================
# MCTS
# ============================================================

class MCTSNode:

    __slots__ = (
        "state",
        "parent",
        "action",
        "prior",
        "visit_count",
        "value_sum",
        "children",
    )

    def __init__(
        self,
        state: State,
        parent: Optional["MCTSNode"] = None,
        action: Optional[int] = None,
        prior: float = 0.0,
    ):
        self.state = state
        self.parent = parent
        self.action = action
        self.prior = prior

        self.visit_count = 0
        self.value_sum = 0.0

        self.children: dict[int, MCTSNode] = {}

    @property
    def value(self) -> float:
        if self.visit_count == 0:
            return 0.0

        return self.value_sum / self.visit_count


class MCTS:

    def __init__(
        self,
        game: Intransitive,
        network: PolicyValueNet,
        simulations: int = 400,
        c_puct: float = 1.5,
        device: str | torch.device = "cpu",
    ):
        self.game = game
        self.network = network
        self.simulations = simulations
        self.c_puct = c_puct
        self.device = torch.device(device)

    # --------------------------------------------------------
    # Neural network evaluation
    # --------------------------------------------------------

    @torch.no_grad()
    def evaluate(self, state: State):

        observation = self.game.observation(state)

        x = torch.from_numpy(
            observation
        ).unsqueeze(0).to(self.device)

        logits, value = self.network(x)

        logits = logits[0].cpu().numpy()
        value = float(value.item())

        # Mask illegal moves.
        legal = self.game.legal_actions(state)

        masked_logits = np.full(
            N_ACTIONS,
            -np.inf,
            dtype=np.float32,
        )

        masked_logits[legal] = logits[legal]

        # Stable softmax.
        max_logit = np.max(
            masked_logits[legal]
        )

        probabilities = np.zeros(
            N_ACTIONS,
            dtype=np.float32,
        )

        exp_logits = np.exp(
            masked_logits[legal] - max_logit
        )

        probabilities[legal] = (
            exp_logits /
            np.sum(exp_logits)
        )

        return probabilities, value

    # --------------------------------------------------------
    # PUCT selection
    # --------------------------------------------------------

    def select_child(
        self,
        node: MCTSNode,
    ) -> MCTSNode:

        if not node.children:
            raise RuntimeError(
                "select_child() called on a node with no children."
            )

        best_score = -float("inf")
        best_child: MCTSNode | None = None

        sqrt_parent_visits = math.sqrt(
            max(1, node.visit_count)
        )

        for child in node.children.values():

            q = -child.value

            exploration = (
                self.c_puct
                * child.prior
                * sqrt_parent_visits
                / (1 + child.visit_count)
            )

            score = q + exploration

            if score > best_score:
                best_score = score
                best_child = child

        # Pylance can now prove this is non-None because
        # node.children was non-empty.
        assert best_child is not None

        return best_child

    # --------------------------------------------------------
    # Expansion
    # --------------------------------------------------------

    def expand(
        self,
        node: MCTSNode,
        priors: np.ndarray,
    ):

        legal = self.game.legal_actions(
            node.state
        )

        for action in legal:

            action = int(action)

            if action in node.children:
                continue

            child_state = self.game.apply(
                node.state,
                action,
                validate=False,
            )

            node.children[action] = MCTSNode(
                state=child_state,
                parent=node,
                action=action,
                prior=float(priors[action]),
            )

    # --------------------------------------------------------
    # Backpropagation
    # --------------------------------------------------------

    @staticmethod
    def backpropagate(
        node: MCTSNode,
        value: float,
    ):

        current = node

        while current is not None:

            current.visit_count += 1
            current.value_sum += value

            # Change perspective.
            value = -value

            current = current.parent

    # --------------------------------------------------------
    # Search
    # --------------------------------------------------------

    def search(
        self,
        root: MCTSNode,
    ) -> None:

        for _ in range(self.simulations):

            node = root

            # ----------------------------------------------
            # Selection
            # ----------------------------------------------

            while node.children:

                node = self.select_child(node)

            state = node.state

            # ----------------------------------------------
            # Terminal state
            # ----------------------------------------------

            if self.game.terminal(state):

                # Value from perspective of player whose
                # turn it WOULD have been.
                value = self.game.reward(
                    state,
                    state.player,
                )

                self.backpropagate(
                    node,
                    value,
                )

                continue

            # ----------------------------------------------
            # Expansion + evaluation
            # ----------------------------------------------

            priors, value = self.evaluate(state)

            self.expand(
                node,
                priors,
            )

            # value is from current state's player perspective.
            self.backpropagate(
                node,
                value,
            )

    # --------------------------------------------------------
    # Search policy
    # --------------------------------------------------------

    def policy(
        self,
        state: State,
        temperature: float = 1.0,
        dirichlet_alpha: float = 0.3,
        dirichlet_eps: float = 0.25,
        add_noise: bool = False,
    ) -> np.ndarray:

        root = MCTSNode(state)

        # ----------------------------------------------------
        # Root network evaluation
        # ----------------------------------------------------

        priors, _ = self.evaluate(state)

        # ----------------------------------------------------
        # Optional AlphaZero-style root exploration noise
        # ----------------------------------------------------

        if add_noise:

            legal_actions = self.game.legal_actions(
                state
            )

            noise = np.random.dirichlet(
                [dirichlet_alpha] * len(legal_actions)
            )

            priors = priors.copy()

            priors[legal_actions] = (
                (1.0 - dirichlet_eps)
                * priors[legal_actions]
                + dirichlet_eps
                * noise
            )

            # Normalize legal priors for numerical cleanliness.
            priors[legal_actions] /= (
                priors[legal_actions].sum()
            )

        # ----------------------------------------------------
        # Expand root
        # ----------------------------------------------------

        self.expand(
            root,
            priors,
        )

        # ----------------------------------------------------
        # MCTS search
        # ----------------------------------------------------

        self.search(root)

        # ----------------------------------------------------
        # Convert visit counts into policy
        # ----------------------------------------------------

        visit_counts = np.zeros(
            N_ACTIONS,
            dtype=np.float32,
        )

        for action, child in root.children.items():
            visit_counts[action] = child.visit_count

        legal = np.flatnonzero(
            visit_counts
        )

        if len(legal) == 0:
            raise RuntimeError(
                "MCTS found no legal moves."
            )

        # ----------------------------------------------------
        # Deterministic policy
        # ----------------------------------------------------

        if temperature <= 1e-8:

            best = legal[
                np.argmax(
                    visit_counts[legal]
                )
            ]

            result = np.zeros(
                N_ACTIONS,
                dtype=np.float32,
            )

            result[best] = 1.0

            return result

        # ----------------------------------------------------
        # Temperature-scaled policy
        # ----------------------------------------------------

        counts = (
            visit_counts[legal]
            ** (1.0 / temperature)
        )

        counts /= counts.sum()

        result = np.zeros(
            N_ACTIONS,
            dtype=np.float32,
        )

        result[legal] = counts

        return result

    # --------------------------------------------------------
    # Select move
    # --------------------------------------------------------

    def select_action(
        self,
        state: State,
        temperature: float = 1.0,
        add_noise: bool = False,
    ) -> tuple[int, np.ndarray]:

        policy = self.policy(
            state=state,
            temperature=temperature,
            add_noise=add_noise,
        )

        action = int(
            np.random.choice(
                N_ACTIONS,
                p=policy,
            )
        )

        return action, policy


# ============================================================
# Self-play
# ============================================================

@dataclass(slots=True)
class TrainingExample:
    observation: np.ndarray
    policy: np.ndarray
    player: int
    legal_mask: np.ndarray

class ReplayBuffer:

    def __init__(self, capacity: int = 100_000):
        self.capacity = capacity
        self.examples = []

    def add_game(self, examples, winner):
        for example in examples:
            self.examples.append(
                (
                    example,
                    winner,
                )
            )

        if len(self.examples) > self.capacity:
            self.examples = self.examples[-self.capacity:]

    def __len__(self):
        return len(self.examples)

    def sample(self, batch_size: int):
        indices = np.random.choice(
            len(self.examples),
            size=batch_size,
            replace=False,
        )

        examples = [
            self.examples[i][0]
            for i in indices
        ]

        winners = [
            self.examples[i][1]
            for i in indices
        ]

        return examples, winners


def self_play_game(
    game: Intransitive,
    mcts: MCTS,
    temperature_moves: int = 20,
    debug: bool = False,
) -> tuple[list[TrainingExample], int]:

    state = game.initial_state()

    examples = []

    while not game.terminal(state):

        temperature = (
            1.0
            if state.move_count < temperature_moves
            else 0.0
        )

        action, policy = mcts.select_action(
            state,
            temperature=temperature,
            add_noise=True,
        )

        # ----------------------------------------------------
        # Debug: print selected move.
        # ----------------------------------------------------

        if debug:
            source, direction = decode_action(action)
            destination = action_destination(action)

            print(
                f"Move {state.move_count + 1:3d}: "
                f"{player_name(state.player):4s} "
                f"{square_to_coord(source)} -> "
                f"{square_to_coord(destination)}"
            )

        examples.append(
            TrainingExample(
                observation=game.observation(state),
                policy=policy,
                player=state.player,
                legal_mask=game.legal_mask(state),
            )
        )

        state = game.apply(
            state,
            action,
            validate=True,
        )

        # ----------------------------------------------------
        # Debug: print board every 10 moves.
        # ----------------------------------------------------

        if debug and (
            state.move_count % 10 == 0
            or game.terminal(state)
        ):
            print()
            print(
                f"Board after move {state.move_count} "
                f"(no capture: {state.no_capture_count}):"
            )
            state.render()
            print()

    # --------------------------------------------------------
    # Debug: final result.
    # --------------------------------------------------------

    if debug:
        if state.winner == BLUE:
            print("RESULT: Blue wins")

        elif state.winner == RED:
            print("RESULT: Red wins")

        elif state.winner == DRAW:
            print("RESULT: Draw")

        print(
            f"Final move count: {state.move_count}"
        )
        print(
            f"Final no-capture count: "
            f"{state.no_capture_count}"
        )

    return examples, state.winner



# ============================================================
# Training batch conversion
# ============================================================

def batch_from_replay(
    examples,
    winners,
    device,
):
    observations = np.stack([
        e.observation
        for e in examples
    ])

    policies = np.stack([
        e.policy
        for e in examples
    ])

    legal_masks = np.stack([
        e.legal_mask
        for e in examples
    ])

    assert np.all(
        legal_masks.sum(axis=1) > 0
    )

    assert np.all(
        policies[~legal_masks] == 0
    )

    values = np.array([
        0.0
        if winner == DRAW
        else (
            1.0
            if winner == e.player
            else -1.0
        )
        for e, winner in zip(examples, winners)
    ], dtype=np.float32)

    observations = torch.from_numpy(
        observations
    ).to(device)

    policies = torch.from_numpy(
        policies
    ).to(device)

    legal_masks = torch.from_numpy(
        legal_masks
    ).to(device)

    values = torch.from_numpy(
        values
    ).to(device)

    return (
        observations,
        policies,
        values,
        legal_masks,
    )

# ============================================================
# AlphaZero loss
# ============================================================

def alphazero_loss(
    policy_logits: torch.Tensor,
    values: torch.Tensor,
    target_policy: torch.Tensor,
    target_value: torch.Tensor,
    legal_mask: torch.Tensor,
):
    """
    AlphaZero loss:

        L =
            cross_entropy(policy)
            +
            MSE(value)

    Illegal actions are masked out of the policy
    distribution before computing log-softmax.
    """

    masked_logits = policy_logits.masked_fill(
        ~legal_mask,
        float("-inf"),
    )

    log_policy = F.log_softmax(
        masked_logits,
        dim=1,
    )

    policy_loss = -(
        target_policy * log_policy
    ).sum(dim=1).mean()

    value_loss = F.mse_loss(
        values,
        target_value,
    )

    return policy_loss + value_loss, {
        "policy_loss": float(
            policy_loss.detach()
        ),
        "value_loss": float(
            value_loss.detach()
        ),
    }

def validate_initial_position(game: Intransitive) -> None:
    state = game.initial_state()

    assert state.player == BLUE
    assert state.winner == ONGOING
    assert state.move_count == 0
    assert state.no_capture_count == 0

    # Exactly 10 pieces per player.
    red_count = np.count_nonzero(
        np.isin(
            state.board,
            [RED_PAPER, RED_ROCK, RED_SCISSORS],
        )
    )

    blue_count = np.count_nonzero(
        np.isin(
            state.board,
            [BLUE_PAPER, BLUE_ROCK, BLUE_SCISSORS],
        )
    )

    assert red_count == 10, red_count
    assert blue_count == 10, blue_count

    # Verify the exact board.
    expected = np.zeros(
        N_SQUARES,
        dtype=np.uint8,
    )

    for x, y, piece in INITIAL_POSITIONS:
        expected[y * BOARD_SIZE + x] = piece

    np.testing.assert_array_equal(
        state.board,
        expected,
    )

    # Verify goals.
    assert RED_GOAL == 0
    assert BLUE_GOAL == 80

    print("Initial position validation: OK")

FILES = "abcdefghi"


def coord_to_square(coord: str) -> Optional[int]:
    """
    Convert coordinates such as 'd4' to a square index.

    a1 = (0, 0)
    i9 = (8, 8)
    """
    coord = coord.lower().strip()

    if len(coord) != 2:
        return None

    file_char, rank_char = coord

    if file_char not in FILES:
        return None

    if rank_char not in "123456789":
        return None

    x = FILES.index(file_char)
    y = int(rank_char) - 1

    return y * BOARD_SIZE + x


def square_to_coord(square: int) -> str:
    """Convert a square index to coordinates such as 'd4'."""
    x = square % BOARD_SIZE
    y = square // BOARD_SIZE

    return f"{FILES[x]}{y + 1}"


def coordinates_to_action(
    source: int,
    destination: int,
) -> Optional[int]:
    """
    Convert source/destination squares into the game's
    encoded action.

    Returns None if the destination is not one king move
    away from the source.
    """
    sx = source % BOARD_SIZE
    sy = source // BOARD_SIZE

    dx = destination % BOARD_SIZE
    dy = destination // BOARD_SIZE

    delta_x = dx - sx
    delta_y = dy - sy

    for direction, (ddx, ddy) in enumerate(DIRECTIONS):
        if delta_x == ddx and delta_y == ddy:
            return encode_action(source, direction)

    return None


def human_move(
    game: Intransitive,
    state: State,
    move_text: str,
) -> bool:
    """
    Parse and apply a human move such as:

        d4 d5

    Returns True if the move was successfully played.
    """

    parts = move_text.strip().lower().split()

    if len(parts) != 2:
        print("Move impossible. Enter moves like: d4 d5")
        return False

    source = coord_to_square(parts[0])
    destination = coord_to_square(parts[1])

    if source is None or destination is None:
        print(
            "Move impossible. "
            "Coordinates must use a-i and 1-9."
        )
        return False

    action = coordinates_to_action(
        source,
        destination,
    )

    if action is None:
        print("Move impossible.")
        return False

    # Use the actual game engine's legality check.
    if not game.is_legal(state, action):
        print("Move impossible.")
        return False

    # Apply the move through the actual game engine.
    new_state = game.apply(
        state,
        action,
        validate=False,
    )

    # Update the current state.
    state.board = new_state.board
    state.player = new_state.player
    state.winner = new_state.winner
    state.move_count = new_state.move_count
    state.no_capture_count = new_state.no_capture_count
    
    return True


def print_coordinate_board(state: State):
    """
    Display the board with coordinates.

    a-i = x coordinate
    1-9 = y coordinate

    y=1 is the bottom row.
    y=9 is the top row.
    """

    symbols = {
        EMPTY: ".",
        RED_PAPER: "p",
        RED_ROCK: "r",
        RED_SCISSORS: "s",
        BLUE_PAPER: "P",
        BLUE_ROCK: "R",
        BLUE_SCISSORS: "S",
    }

    print()
    print("     " + " ".join(FILES))

    for y in range(BOARD_SIZE - 1, -1, -1):

        row = []

        for x in range(BOARD_SIZE):

            square = y * BOARD_SIZE + x

            if square == BLUE_GOAL:
                row.append("T")

            elif square == RED_GOAL:
                row.append("t")

            else:
                row.append(
                    symbols[state.board[square]]
                )

        print(
            f"{y + 1:2}   "
            + " ".join(row)
        )

    print()


def player_name(player: int) -> str:
    if player == BLUE:
        return "Blue"

    return "Red"


def play_human_game():
    game = Intransitive()
    state = game.initial_state()

    print("===================================")
    print("       INTRANSITIVE TEST GAME")
    print("===================================")
    print()
    print("Blue moves first.")
    print("Enter moves as: d4 d5")
    print("Coordinates: a-i horizontally, 1-9 vertically.")
    print("Type 'quit' to exit.")
    print()

    while True:

        print_coordinate_board(state)

        # Use the existing game engine.
        if game.terminal(state):

            if state.winner == BLUE:
                print("Blue wins!")

            elif state.winner == RED:
                print("Red wins!")

            elif state.winner == DRAW:
                print("Draw!")

            break

        print(
            f"{player_name(state.player)} to move."
        )

        move_text = input("> ").strip()

        if move_text.lower() in (
            "quit",
            "exit",
            "q",
        ):
            print("Game ended.")
            break

        if human_move(
            game,
            state,
            move_text,
        ):
            print(
                f"{move_text.lower()} played."
            )

def train_step(
    network,
    optimizer,
    examples,
    winners,
    device,
):
    network.train()

    (
        observations,
        policies,
        values,
        legal_masks,
    ) = batch_from_replay(
        examples,
        winners,
        device,
    )

    optimizer.zero_grad(
        set_to_none=True
    )

    policy_logits, predicted_values = network(
        observations
    )

    loss, metrics = alphazero_loss(
        policy_logits,
        predicted_values,
        policies,
        values,
        legal_masks,
    )

    loss.backward()

    torch.nn.utils.clip_grad_norm_(
        network.parameters(),
        max_norm=5.0,
    )

    optimizer.step()

    return float(loss.detach()), metrics

def train_alphazero(
    iterations: int = 100,
    games_per_iteration: int = 100,
    simulations: int = 400,
    batch_size: int = 128,
    training_steps: int = 20,
    replay_capacity: int = 100_000,
    learning_rate: float = 1e-3,
    device: str | torch.device = (
        "cuda" if torch.cuda.is_available() else "cpu"
    ),
):

    device = torch.device(device)

    print("===================================")
    print("       INTRANSITIVE ALPHAZERO")
    print("===================================")
    print()
    print(f"Device: {device}")
    print(f"MCTS simulations: {simulations}")
    print(f"Games / iteration: {games_per_iteration}")
    print(f"Batch size: {batch_size}")
    print(f"Training steps: {training_steps}")
    print()

    # --------------------------------------------------------
    # Validation
    # --------------------------------------------------------

    game = Intransitive()

    validate_initial_position(game)

    # --------------------------------------------------------
    # Network
    # --------------------------------------------------------

    network = PolicyValueNet(
        channels=64,
        num_blocks=5,
    ).to(device)

    # --------------------------------------------------------
    # Optimizer
    # --------------------------------------------------------

    optimizer = torch.optim.AdamW(
        network.parameters(),
        lr=learning_rate,
        weight_decay=1e-4,
    )

    # --------------------------------------------------------
    # MCTS
    # --------------------------------------------------------

    mcts = MCTS(
        game=game,
        network=network,
        simulations=simulations,
        c_puct=1.5,
        device=device,
    )

    # --------------------------------------------------------
    # Replay buffer
    # --------------------------------------------------------

    replay = ReplayBuffer(
        capacity=replay_capacity
    )

    # --------------------------------------------------------
    # Training loop
    # --------------------------------------------------------

    for iteration in range(
        1,
        iterations + 1,
    ):

        print()
        print(
            f"========== Iteration {iteration} / "
            f"{iterations} =========="
        )

        # ====================================================
        # Self-play
        # ====================================================

        network.eval()

        total_examples = 0
        blue_wins = 0
        red_wins = 0
        draws = 0
        game_lengths = []

        for game_number in range(
            1,
            games_per_iteration + 1,
        ):

            examples, winner = self_play_game(
                game,
                mcts,
                temperature_moves=20,
                debug=False#(iteration == 1 and game_number == 1),
            )

            game_lengths.append(len(examples))

            replay.add_game(
                examples,
                winner,
            )

            total_examples += len(examples)

            if winner == BLUE:
                blue_wins += 1

            elif winner == RED:
                red_wins += 1

            else:
                draws += 1

            if game_number % 10 == 0:
                print(
                    f"  Self-play: "
                    f"{game_number}/{games_per_iteration}"
                )

        print(
            f"  Games: "
            f"Blue {blue_wins} | "
            f"Red {red_wins} | "
            f"Draws {draws}"
        )

        print(
            f"  Game length: "
            f"min {min(game_lengths)} | "
            f"avg {np.mean(game_lengths):.1f} | "
            f"max {max(game_lengths)}"
        )

        print(
            f"  New examples: {total_examples}"
        )

        print(
            f"  Replay buffer: {len(replay)}"
        )

        # ====================================================
        # Training
        # ====================================================

        if len(replay) < batch_size:
            print(
                "  Not enough replay data for training."
            )
            continue

        total_loss = 0.0
        total_policy_loss = 0.0
        total_value_loss = 0.0

        for step in range(
            1,
            training_steps + 1,
        ):

            examples, winners = replay.sample(
                batch_size
            )

            loss, metrics = train_step(
                network,
                optimizer,
                examples,
                winners,
                device,
            )

            total_loss += loss
            total_policy_loss += metrics["policy_loss"]
            total_value_loss += metrics["value_loss"]

        print(
            f"  Loss: "
            f"{total_loss / training_steps:.4f}"
        )

        print(
            f"  Policy loss: "
            f"{total_policy_loss / training_steps:.4f}"
        )

        print(
            f"  Value loss: "
            f"{total_value_loss / training_steps:.4f}"
        )

        # ====================================================
        # Checkpoint
        # ====================================================

        checkpoint = {
            "iteration": iteration,
            "network": network.state_dict(),
            "optimizer": optimizer.state_dict(),
        }

        torch.save(
            checkpoint,
            f"intransitive_checkpoint_{iteration:04d}.pt",
        )

        print(
            f"  Saved checkpoint "
            f"intransitive_checkpoint_{iteration:04d}.pt"
        )

    return network


if __name__ == "__main__":

    train_alphazero(
        iterations=100,
        games_per_iteration=100,
        simulations=400,
        batch_size=32,
        training_steps=20,
    )