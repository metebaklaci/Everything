// inputcompute.cpp

#include "inputcompute.h"

using namespace std;

double linear(double input){
    return input;
}

double relu(double input){
    return (input > 0) ? input : 0;
}

double lrelu(double input){
    return (input > 0) ? input : 0.1 * input;
}

// Numerically stable sigmoid
double sigmoid(double input){

    if(input >= 0){

        double z = exp(-input);
        return 1.0 / (1.0 + z);
    }

    double z = exp(input);
    return z / (1.0 + z);
}

double activate(char activation, double input){

    switch(activation){

        case 'x':
            return linear(input);

        case 's':
            return sigmoid(input);

        case 'r':
            return relu(input);

        case 'l':
            return lrelu(input);

        case 't':
            return tanh(input);

        default:
            throw runtime_error(
                "Unknown activation function."
            );
    }
}

vector<node_value_pair> compute(
    Graph& agent,
    vector<node_value_pair> inputs
){

    // Allocate once with correct size
    vector<double> node_values(
        agent.nodes.size(),
        0.0
    );

    // Load inputs
    for(const node_value_pair& input : inputs){

        if(
            !agent.node_id_to_index.count(input.id)
        ){
            throw runtime_error(
                "Input node ID not found."
            );
        }

        int index =
            agent.node_id_to_index[input.id];

        if(agent.nodes[index].type != 'i'){

            throw runtime_error(
                "Tried to assign input to non-input node."
            );
        }

        node_values[index] = input.value;
    }

    // Find max depth
    int maxdepth = 0;

    for(const Graph::Node& n : agent.nodes){

        if(n.depth > maxdepth)
            maxdepth = n.depth;
    }

    // Forward pass
    for(int depth = 1; depth <= maxdepth; ++depth){

        for(int node_index = 0;
            node_index < agent.nodes.size();
            ++node_index){

            Graph::Node& node =
                agent.nodes[node_index];

            if(node.depth != depth)
                continue;

            double value = node.bias;

            // Use adjacency structure instead
            for(
                int conn_index :
                agent.incoming_connections[node.id]
            ){

                const Graph::Connection& conn =
                    agent.connections[conn_index];

                if(!conn.enabled)
                    continue;

                value +=
                    node_values[
                        agent.node_id_to_index[
                            conn.nodein
                        ]
                    ] * conn.weight;
            }

            node_values[node_index] =
                activate(
                    node.activation,
                    value
                );
        }
    }

    // Gather outputs
    vector<node_value_pair> outputs;

    for(int i = 0; i < agent.nodes.size(); ++i){

        if(agent.nodes[i].type == 'o'){

            outputs.push_back({
                agent.nodes[i].id,
                node_values[i]
            });
        }
    }

    return outputs;
}