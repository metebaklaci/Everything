import math
import copy
import random

MOVE_NAMES = ["UP", "LEFT", "DOWN", "RIGHT"]


class Game2048:

    def __init__(self, board=None):
        """
        Create a new 2048 game.

        Parameters
        ----------
        board : list[list[int]] | None
            Optional 4x4 board. If None, an empty board is created.
        """

        if board is None:
            self.board = [[0] * 4 for _ in range(4)]
        else:
            if len(board) != 4 or any(len(row) != 4 for row in board):
                raise ValueError("Board must be a 4x4 list.")

            # Deep copy so modifying the input doesn't affect the game.
            self.board = [row[:] for row in board]

    def print(self):

        print()

        for row in self.board:
            print(" ".join(f"{x:5}" if x else "    ." for x in row))

        print()

    def get_board_flat(self):

        return [x for row in self.board for x in row]

    def add_tile(self, row, col, value):

        if self.board[row][col] != 0:
            raise ValueError("Square already occupied.")

        if value not in (2, 4):
            raise ValueError("Tile must be 2 or 4.")

        self.board[row][col] = value

    def compress(self, row):

        row = [x for x in row if x != 0]

        merged = []
        score = 0

        i = 0

        while i < len(row):

            if i + 1 < len(row) and row[i] == row[i + 1]:

                value = row[i] * 2
                merged.append(value)
                score += value
                i += 2

            else:

                merged.append(row[i])
                i += 1

        merged += [0] * (4 - len(merged))

        return merged, score

    def move_left(self):

        moved = False
        total_score = 0
        new_board = []

        for row in self.board:

            compressed, score = self.compress(row)

            total_score += score

            if compressed != row:
                moved = True

            new_board.append(compressed)

        self.board = new_board

        return moved, total_score

    def move_right(self):

        moved = False
        total_score = 0
        new_board = []

        for row in self.board:

            reversed_row = row[::-1]

            compressed, score = self.compress(reversed_row)

            compressed = compressed[::-1]

            if compressed != row:
                moved = True

            total_score += score
            new_board.append(compressed)

        self.board = new_board

        return moved, total_score

    def transpose(self):

        self.board = [list(row) for row in zip(*self.board)]

    def move_up(self):

        self.transpose()

        moved, score = self.move_left()

        self.transpose()

        return moved, score

    def move_down(self):

        self.transpose()

        moved, score = self.move_right()

        self.transpose()

        return moved, score

    def move(self, action):

        if action == 0:
            return self.move_up()

        elif action == 1:
            return self.move_left()

        elif action == 2:
            return self.move_down()

        elif action == 3:
            return self.move_right()

        else:
            raise ValueError("Invalid action.")

    def game_over(self):

        # Empty square
        for row in self.board:
            if 0 in row:
                return False

        # Horizontal merge
        for r in range(4):
            for c in range(3):
                if self.board[r][c] == self.board[r][c + 1]:
                    return False

        # Vertical merge
        for r in range(3):
            for c in range(4):
                if self.board[r][c] == self.board[r + 1][c]:
                    return False

        return True
    

def clone(game):

    g = Game2048()
    g.board = [row[:] for row in game.board]
    return g


def chance_value(game, depth, accumulated_score):

    empty = []

    for r in range(4):
        for c in range(4):
            if game.board[r][c] == 0:
                empty.append((r, c))

    if not empty:
        return max_value(game, depth - 1, accumulated_score)

    p_cell = 1.0 / len(empty)

    expected = 0.0

    for r, c in empty:

        for value, p_tile in ((2, 0.9), (4, 0.1)):

            child = clone(game)
            child.board[r][c] = value

            expected += (
                p_cell
                * p_tile
                * max_value(child, depth - 1, accumulated_score)
            )

    return expected


def max_value(game, depth, accumulated_score):

    if game.game_over():
        return -1.0

    if depth == 0:
        return math.log(accumulated_score + 1)

    best = -float("inf")

    legal = False

    for action in range(4):

        child = clone(game)

        moved, gained = child.move(action)

        if not moved:
            continue

        legal = True

        value = chance_value(
            child,
            depth,
            accumulated_score + gained
        )

        if value > best:
            best = value

    if not legal:
        return -1.0

    return best


def expectimax_best_move(game, depth):

    best_move = None
    best_value = -float("inf")

    for action in range(4):

        child = clone(game)

        moved, gained = child.move(action)

        if not moved:
            continue

        value = chance_value(
            child,
            depth,
            gained
        )

        if value > best_value:
            best_value = value
            best_move = action

    return best_move, best_value


def spawn_random_tile(game):
    """
    Spawn a random tile according to standard 2048 rules.

    Returns True if a tile was spawned.
    """

    empty = []

    for r in range(4):
        for c in range(4):
            if game.board[r][c] == 0:
                empty.append((r, c))

    if not empty:
        return False

    r, c = random.choice(empty)

    game.board[r][c] = 2 if random.random() < 0.9 else 4

    return True


def new_game():

    game = Game2048()

    spawn_random_tile(game)
    spawn_random_tile(game)

    return game


def autoplay(depth=3, print_board=True):

    game = new_game()

    total_score = 0
    moves = 0

    if print_board:
        print("Initial board")
        game.print()

    while not game.game_over():

        action, value = expectimax_best_move(game, depth)

        # No legal moves
        if action is None:
            break

        moved, gained = game.move(action)

        if not moved:
            break

        total_score += gained
        moves += 1

        spawn_random_tile(game)

        if print_board:

            print(
                f"Move {moves}: "
                f"{['UP','LEFT','DOWN','RIGHT'][action]}   "
                f"+{gained}   "
                f"Total={total_score}   "
                f"Expectimax={value:.4f}"
            )

            game.print()

    print("Game over")
    print("Moves:", moves)
    print("Final score:", total_score)
    print("Highest tile:", max(max(row) for row in game.board))

    return game, total_score


game, score = autoplay(depth=3)