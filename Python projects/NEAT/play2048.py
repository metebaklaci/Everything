from emulate2048 import Board
from expectimax import Expectimax, trivialscoreeval


MOVE_NAMES = {
    Board.UP: "UP",
    Board.LEFT: "LEFT",
    Board.DOWN: "DOWN",
    Board.RIGHT: "RIGHT"
}


def print_board(board):
    print("\nScore:", board.score)

    values = np.where(
        board.board == 0,
        0,
        1 << board.board
    )

    for row in values:
        print(" ".join(f"{v:5}" if v else "    ." for v in row))

    print()


def main():

    board = Board()

    ai = Expectimax(
        depth=3,
        evaluator=trivialscoreeval
    )

    move_count = 0

    print_board(board)

    while not board.game_over():

        move = ai.best_move(board)

        if move is None:
            break

        print(f"Move {move_count + 1}: {MOVE_NAMES[move]}")

        changed = board.move(move)

        if changed:
            board.spawn_random_tile()

        print_board(board)

        move_count += 1

    print("Game Over")
    print("Moves:", move_count)
    print("Score:", board.score)
    print("Max Tile:", board.max_tile())


if __name__ == "__main__":
    import numpy as np
    main()