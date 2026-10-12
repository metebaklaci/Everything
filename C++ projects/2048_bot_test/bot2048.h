#ifndef BOT2048_H
#define BOT2048_H

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <chrono>
#include <thread>



struct board{
    std::vector< std::vector<int> > grid = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
    int points = 0;
};

struct minimaxreturn{
    double nodevalue = 0;
    int move = -1; //default value is nonsensical on purpose
};

void prettyprint(board& b);

void moveboard(board& brd, int move, int rowcol);

bool mergeboard(board& brd, int move, int rowcol);

board randomspawn(board brd, int randin);

bool emulate(board& brd, int move, bool spawn);

double eval(const board& b);

minimaxreturn minimax(board b, int currentdepth, int maxdepth);

#endif