#ifndef NEAT_H
#define NEAT_H

#include <bits/stdc++.h>

struct connectiongene{
    int id = 0;
    int node_in = 0;
    int node_out = 0;
    double weight = 0;
    bool enabled = true;
};

struct node{
    int id = 0;
    char type = 'i'; //'i' = input 'h' = hidden 'o' = output
};

struct agent{
    std::vector<connectiongene> connectiongenome;
    std::vector<node> nodes;
};

#endif