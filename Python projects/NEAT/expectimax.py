from emulate2048 import Board

MAX_PLAYER = 0
CHANCE_PLAYER = 1


class Expectimax:
    def __init__(self, depth, evaluator):
        """
        Parameters
        ----------
        depth : int
            Search depth.
        evaluator : callable
            Function taking a Board and returning a scalar evaluation.
        """
        self.depth = depth
        self.evaluator = evaluator

    def search(self, board, depth, player):
        """
        Returns the expectimax value of a position.
        """

        if depth == 0 or board.game_over():
            return self.evaluator(board)

        if player == MAX_PLAYER:

            best = float("-inf")
            found_move = False

            for move in (
                Board.UP,
                Board.LEFT,
                Board.DOWN,
                Board.RIGHT,
            ):
                child = board.copy()

                if not child.move(move):
                    continue

                found_move = True

                value = self.search(
                    child,
                    depth - 1,
                    CHANCE_PLAYER
                )

                best = max(best, value)

            if not found_move:
                return self.evaluator(board)

            return best

        else:

            empty = board.empty_cells()

            if not empty:
                return self.search(
                    board,
                    depth - 1,
                    MAX_PLAYER
                )

            total = 0.0

            for cell in empty:

                child = board.copy()

                child.place_tile(cell)

                total += self.search(
                    child,
                    depth - 1,
                    MAX_PLAYER
                )

            return total / len(empty)

    def best_move(self, board):
        """
        Returns the best move from the current board.
        """

        best_move = None
        best_value = float("-inf")

        for move in (
            Board.UP,
            Board.LEFT,
            Board.DOWN,
            Board.RIGHT,
        ):

            child = board.copy()

            if not child.move(move):
                continue

            value = self.search(
                child,
                self.depth - 1,
                CHANCE_PLAYER
            )

            if value > best_value:
                best_value = value
                best_move = move

        return best_move
    

def trivialscoreeval(board):
    return board.score


