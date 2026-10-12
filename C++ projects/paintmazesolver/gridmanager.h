#ifndef GRIDMANAGER_H

#define GRIDMANAGER_H

#include <iostream>
#include <vector>
#include <queue>

struct grid{
    std::vector< std::vector<char> > map = {}; // '#' = blocked     'O' = open     'P' = painted     The program expects the margins of the map to be blocked.
    int player_x = 0;
    int player_y = 0;
    char metadata = '1'; //Used for move possibility identification
    std::vector<char> move_sequence = {};
};

bool painted(grid g);
grid move(grid g, char move);
void prettyprint(grid g);

#endif