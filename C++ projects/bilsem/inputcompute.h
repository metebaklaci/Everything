// inputcompute.h

#ifndef INPUTCOMPUTE_H
#define INPUTCOMPUTE_H

#include <vector>
#include "agent.h"

struct node_value_pair{
    int id;
    double value;
};

double linear(double input);
double relu(double input);
double lrelu(double input);
double sigmoid(double input);

double activate(char activation, double input);

std::vector<node_value_pair> compute(
    Graph& agent,
    std::vector<node_value_pair> inputs
);

#endif