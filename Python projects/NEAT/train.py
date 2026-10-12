import pickle
import neat

from emulate2048 import Board
from expectimax import Expectimax


SEARCH_DEPTH = 1
GAMES_PER_GENOME = 10

# Tracks which generation is currently being evaluated.
CURRENT_GENERATION = 0


class NeuralEvaluator:
    """
    Wraps a NEAT network so it can be used as
    an Expectimax evaluation function.
    """

    def __init__(self, net):
        self.net = net

    def __call__(self, board):

        inputs = board.encode() / 16.0

        return self.net.activate(inputs)[0]


def play_game(net, seed):
    """
    Plays one complete game.

    Returns the final score.
    """

    board = Board(seed=seed)

    evaluator = NeuralEvaluator(net)

    ai = Expectimax(
        depth=SEARCH_DEPTH,
        evaluator=evaluator
    )

    while not board.game_over():

        move = ai.best_move(board)

        if move is None:
            break

        moved = board.move(move)

        if moved:
            board.spawn_random_tile()

    return board.score


def eval_genomes(genomes, config):
    """
    Called automatically by neat-python.
    """

    global CURRENT_GENERATION

    # Every generation uses a different set of seeds,
    # but every genome within the generation uses the same ones.
    game_seeds = [
        CURRENT_GENERATION * GAMES_PER_GENOME + i
        for i in range(GAMES_PER_GENOME)
    ]

    for genome_id, genome in genomes:

        net = neat.nn.FeedForwardNetwork.create(
            genome,
            config
        )

        total_score = 0

        for seed in game_seeds:

            total_score += play_game(
                net,
                seed
            )

        genome.fitness = total_score / GAMES_PER_GENOME

    CURRENT_GENERATION += 1


def run():

    config = neat.Config(
        neat.DefaultGenome,
        neat.DefaultReproduction,
        neat.DefaultSpeciesSet,
        neat.DefaultStagnation,
        "config-feedforward.txt"
    )

    population = neat.Population(config)

    population.add_reporter(
        neat.StdOutReporter(True)
    )

    stats = neat.StatisticsReporter()

    population.add_reporter(stats)

    population.add_reporter(
        neat.Checkpointer(
            generation_interval=10,
            filename_prefix="checkpoint-"
        )
    )

    winner = population.run(
        eval_genomes,
        100
    )

    with open("winner.pkl", "wb") as f:
        pickle.dump(winner, f)

    print("\nTraining complete.")
    print("Winner fitness:", winner.fitness)


if __name__ == "__main__":
    run()