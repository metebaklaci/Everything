#include "agent.h"
#include <iomanip>

using namespace std;

double evaluateXOR(Graph& g){

    vector<vector<double>> xor_inputs = {

        {0,0},
        {0,1},
        {1,0},
        {1,1}
    };

    vector<double> xor_outputs = {

        0,
        1,
        1,
        0
    };

    double error = 0.0;

    for(int i = 0; i < 4; ++i){

        vector<node_value_pair> input = {

            {0, xor_inputs[i][0]},
            {1, xor_inputs[i][1]}
        };

        vector<node_value_pair> output =
            g.compute(input);

        double prediction =
            output[0].value;

        double diff =
            xor_outputs[i]
            -
            prediction;

        error += diff * diff;
    }

    /*
        Max fitness = 4
        Perfect XOR => error = 0
    */

    return 4.0 - error;
}

void printWinnerPredictions(Graph& g){

    vector<vector<double>> xor_inputs = {

        {0,0},
        {0,1},
        {1,0},
        {1,1}
    };

    for(int i = 0; i < 4; ++i){

        vector<node_value_pair> input = {

            {0, xor_inputs[i][0]},
            {1, xor_inputs[i][1]}
        };

        auto output =
            g.compute(input);

        cout
            << xor_inputs[i][0]
            << " XOR "
            << xor_inputs[i][1]
            << " = "
            << output[0].value
            << "\n";
    }
}

int main(){

    srand(time(0));

    cout << fixed << setprecision(15);

    // ====================================
    // Base topology
    // ====================================

    vector<Graph::Node> nodes = {

        Graph::Node(
            0,
            'i',
            0,
            0,
            'x'
        ),

        Graph::Node(
            1,
            'i',
            0,
            0,
            'x'
        ),

        Graph::Node(
            2,
            'o',
            1,
            0,
            's'
        )
    };

    vector<Graph::Connection> connections = {

        Graph::Connection(
            0,
            0,
            2,
            0,
            true
        ),

        Graph::Connection(
            1,
            1,
            2,
            0,
            true
        )
    };

    // ====================================
    // Create population
    // ====================================

    const int POP_SIZE = 150;

    vector<Graph> agents;

    for(int i = 0; i < POP_SIZE; ++i){

        Graph g(
            nodes,
            connections
        );

        g.initialize_he();

        g.recompile();

        agents.push_back(g);
    }

    Population pop(

        agents,

        2, // highest node innovation

        1  // highest connection innovation
    );

    pop.finalizePopulation();

    // ====================================
    // Reset log files
    // ====================================

    {
        ofstream clear1(
            "xor_log.txt"
        );

        clear1.close();

        ofstream clear2(
            "xor_detailed.txt"
        );

        clear2.close();
    }

    // ====================================
    // Evolution loop
    // ====================================

    const int GENERATIONS = 1000;

    for(
        int generation = 0;
        generation < GENERATIONS;
        ++generation
    ){

        vector<double> fitnesses;

        double best_fitness = -1e18;

        int best_index = 0;

        // ============================
        // Evaluate population
        // ============================

        for(
            int i = 0;
            i < pop.agents.size();
            ++i
        ){

            double fitness =
                evaluateXOR(
                    pop.agents[i]
                );

            fitnesses.push_back(
                fitness
            );

            if(
                fitness
                >
                best_fitness
            ){

                best_fitness =
                    fitness;

                best_index = i;
            }
        }

        // ============================
        // Speciate BEFORE printing
        // so logs show correct species
        // ============================

        pop.speciate(1.5);

        // ============================
        // Logging
        // ============================

        pop.printCompactToFile(

            fitnesses,

            "xor_log.txt",

            generation,

            true
        );

        pop.printPopulationDetailedToFile(

            fitnesses,

            "xor_detailed.txt",

            true
        );

        // ============================
        // Console summary
        // ============================

        cout
            << "Generation: "
            << generation

            << " | Best Fitness: "
            << best_fitness

            << " | Species: "
            << pop.species.size()

            << "\n";

        // ============================
        // Solution found
        // ============================

        if(best_fitness > 3.99){

            cout
                << "\nXOR SOLVED\n\n";

            cout
                << "Best Fitness: "
                << best_fitness
                << "\n\n";

            Graph& winner =
                pop.agents[
                    best_index
                ];

            printWinnerPredictions(
                winner
            );

            cout << "\n";

            cout
                << "Winner topology:\n";

            cout
                << "Nodes: "
                << winner.nodes.size()
                << "\n";

            cout
                << "Connections: "
                << winner.connections.size()
                << "\n\n";

            cout
                << "Connections:\n";

            for(
                const auto& c :
                winner.connections
            ){

                cout
                    << "ID "
                    << c.id
                    << " : "
                    << c.nodein
                    << " -> "
                    << c.nodeout
                    << " | w="
                    << c.weight
                    << " | enabled="
                    << c.enabled
                    << "\n";
            }

            return 0;
        }

        // ============================
        // Evolve next generation
        // ============================

        pop.evolve(fitnesses);
    }

    cout
        << "\nFailed to solve XOR.\n";

    return 0;
}