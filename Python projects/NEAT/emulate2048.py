import numpy as np
import random


class Board:
    """
    2048 board.

    Internally stores tile exponents.

    0 = empty
    1 = 2
    2 = 4
    3 = 8
    ...
    """

    UP = 0
    LEFT = 1
    DOWN = 2
    RIGHT = 3

    def __init__(self, seed=None):
        self.board = np.zeros((4, 4), dtype=np.int16)
        self.score = 0

        # Independent RNG for this board
        self.rng = random.Random(seed)

        self.spawn_random_tile()
        self.spawn_random_tile()

    def copy(self):
        b = Board.__new__(Board)
        b.board = self.board.copy()
        b.score = self.score

        # RNG is irrelevant for search copies.
        b.rng = None

        return b

    def reset(self):
        self.board.fill(0)
        self.score = 0

        self.spawn_random_tile()
        self.spawn_random_tile()

    def empty_cells(self):
        return list(zip(*np.where(self.board == 0)))
    
    def place_tile(self, position, exponent=1):
        """
        Place a tile (value 2 by default).

        Returns False if the cell is occupied.
        """

        r, c = position

        if self.board[r, c] != 0:
            return False

        self.board[r, c] = exponent
        return True

    def spawn_random_tile(self):
        empty = self.empty_cells()

        if not empty:
            return False

        position = self.rng.choice(empty)
        self.place_tile(position)

        return True

    @staticmethod
    def _merge_row_left(row):
        """
        Merge a single row left.

        Returns:
            new_row
            score_gained
            changed
        """

        tiles = [x for x in row if x != 0]

        merged = []
        score = 0

        i = 0
        while i < len(tiles):

            if i + 1 < len(tiles) and tiles[i] == tiles[i + 1]:
                new_exp = tiles[i] + 1
                merged.append(new_exp)

                score += 1 << new_exp
                i += 2

            else:
                merged.append(tiles[i])
                i += 1

        merged.extend([0] * (4 - len(merged)))

        changed = not np.array_equal(row, merged)

        return np.array(merged, dtype=np.int16), score, changed

    def move_left(self):

        changed = False

        for r in range(4):

            new_row, gained, row_changed = self._merge_row_left(
                self.board[r]
            )

            self.board[r] = new_row
            self.score += gained
            changed |= row_changed

        return changed

    def move_up(self):
        self.board = np.rot90(self.board, 1)
        changed = self.move_left()
        self.board = np.rot90(self.board, -1)
        return changed


    def move_right(self):
        self.board = np.rot90(self.board, 2)
        changed = self.move_left()
        self.board = np.rot90(self.board, 2)
        return changed


    def move_down(self):
        self.board = np.rot90(self.board, -1)
        changed = self.move_left()
        self.board = np.rot90(self.board, 1)
        return changed

    def move(self, direction):

        if direction == Board.LEFT:
            changed = self.move_left()

        elif direction == Board.RIGHT:
            changed = self.move_right()

        elif direction == Board.UP:
            changed = self.move_up()

        elif direction == Board.DOWN:
            changed = self.move_down()

        else:
            raise ValueError("Invalid direction")

        return changed

    def legal_moves(self):

        moves = []

        for direction in (
            Board.UP,
            Board.LEFT,
            Board.DOWN,
            Board.RIGHT,
        ):
            b = self.copy()

            if b.move(direction):
                moves.append(direction)

        return moves

    def game_over(self):

        return len(self.legal_moves()) == 0

    def max_tile(self):

        return 1 << int(np.max(self.board))

    def encode(self):
        """
        Input vector for NEAT.

        Already log2 encoded.
        """

        return self.board.flatten().astype(np.float32)

    def __str__(self):

        values = np.where(
            self.board == 0,
            0,
            1 << self.board
        )

        return str(values)
    

def print_board(board):
    print("\nScore:", board.score)
    print("-" * 25)

    for row in board.board:
        values = [
            "." if x == 0 else str(1 << x)
            for x in row
        ]

        print(" ".join(f"{v:>5}" for v in values))

    print("-" * 25)