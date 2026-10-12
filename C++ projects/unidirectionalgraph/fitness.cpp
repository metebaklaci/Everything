#include "fitness.h"

/*
 * @param results Outputs of the function
 * @param expected Expected outputs
 * results and expected must have the same nodes. Otherwise, runtime error will be thrown.
 */
double fitness(vector<node_value_pair> results, vector<node_value_pair> expected){

    if(results.size() != expected.size()) throw runtime_error("Size mismatch between results and expected.");

    for(int i = 0; i < results.size(); ++i){
        bool flag = false;
        for(int j = 0; j < expected.size(); ++j){
            if(results[i].id == expected[j].id){
                flag = true;
                break;
            }
        }
        if(!flag) throw runtime_error("Mismatch between ids of result nodes and expected nodes");
    }

    //MSE
    double mse = 0;
    for(int i = 0; i < results.size(); ++i){
        for(int j = 0; j < expected.size(); ++j){
            if(results[i].id == expected[j].id){
                mse += (expected[j].value - results[i].value)*(expected[j].value - results[i].value);
            }
        }
    }
    mse /= results.size();
    return -mse; //The MSE is a loss, fitness = -loss
    
}