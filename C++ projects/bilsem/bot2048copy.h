#ifndef BOT2048COPY_H
#define BOT2048COPY_H

#include <array>
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

struct board{

    std::array<std::array<int,4>,4> grid {{
        {{0,0,0,0}},
        {{0,0,0,0}},
        {{0,0,0,0}},
        {{0,0,0,0}}
    }};

    int points = 0;
};

struct minimaxreturn{
    double nodevalue = 0;
    int move = -1;
};

/*
Move encoding:

0 = up
1 = right
2 = down
3 = left
*/

void prettyprint(const board& b);

bool randomspawn(board& b);

bool canMove(const board& b);

bool emulate(board& b, int move, bool spawn = true);

double eval(const board& b);

minimaxreturn minimax(
    board b,
    int currentdepth,
    int maxdepth
);

#endif