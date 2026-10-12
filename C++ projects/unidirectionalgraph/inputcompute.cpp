#include "inputcompute.h"

using namespace std;

double linear(double input){
    return input;
}

double relu(double input){
    if(input > 0) return input;
    else return 0;
}

double lrelu(double input){
    if(input > 0) return input;
    else return 0.1*input;
}

double sigmoid(double input){
    return 1.0 / (1.0 + exp(-input));;
}

vector<node_value_pair> compute(Graph& agent, vector<node_value_pair> inputs){
    vector<double> node_values = {};

    for(int i = 0; i < agent.nodes.size(); ++i){
        node_values.push_back(0);
    }

    for(int i = 0; i < inputs.size(); ++i){
        if(agent.nodes[agent.node_id_to_index[inputs[i].id]].type == 'i'){
            node_values[agent.node_id_to_index[inputs[i].id]] = inputs[i].value;
        } else {
            throw runtime_error("Tried to input value to non-input node. Inputs can only be given to input nodes.");
        }
    }

    int maxdepth = -1;

    for(int i = 0; i < agent.nodes.size(); ++i){
        if(agent.nodes[i].depth > maxdepth) maxdepth = agent.nodes[i].depth;
    }

    for(int i = 1; i <= maxdepth; ++i){
        for(int j = 0; j < agent.nodes.size(); ++j){
            if(agent.nodes[j].depth == i){
                double tempvalue = agent.nodes[j].bias;
                for(int k = 0; k < agent.connections.size(); ++k){
                    if(agent.connections[k].nodeout == agent.nodes[j].id && agent.connections[k].enabled){
                        tempvalue += node_values[agent.node_id_to_index[agent.connections[k].nodein]] * agent.connections[k].weight;
                    }
                }

                if(agent.nodes[j].activation == 'x') node_values[j] = linear(tempvalue);
                else if(agent.nodes[j].activation == 's') node_values[j] = sigmoid(tempvalue);
                else if(agent.nodes[j].activation == 'r') node_values[j] = relu(tempvalue);
                else if(agent.nodes[j].activation == 'l') node_values[j] = lrelu(tempvalue);
                else if(agent.nodes[j].activation == 't') node_values[j] = tanh(tempvalue);
            }
        }
    }

    vector<node_value_pair> outputs = {};
    for(int i = 0; i < agent.nodes.size(); ++i){
        if(agent.nodes[i].type == 'o'){
            node_value_pair pair_out;
            pair_out.id = agent.nodes[i].id;
            pair_out.value = node_values[i];

            outputs.push_back(pair_out);
        }
    }

    return outputs;

}