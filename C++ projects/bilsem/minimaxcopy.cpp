#include "bot2048copy.h"

#include <iostream>
#include <limits>

using namespace std;

static constexpr double NEG_INF =
    -numeric_limits<double>::infinity();

/*
Move encoding:

0 = up
1 = right
2 = down
3 = left
*/

minimaxreturn minimax(
    board b,
    int currentdepth,
    int maxdepth
){

    minimaxreturn result;

    if(currentdepth >= maxdepth){

        result.nodevalue = eval(b);

        return result;
    }

    double best_value = NEG_INF;
    int best_move = -1;

    for(int move = 0; move < 4; ++move){

        board moved = b;

        bool valid =
            emulate(moved, move, false);

        if(!valid)
            continue;

        vector<pair<int,int>> empty;

        for(int r = 0; r < 4; ++r){

            for(int c = 0; c < 4; ++c){

                if(moved.grid[r][c] == 0){

                    empty.push_back({r,c});
                }
            }
        }

        if(empty.empty()){

            double value =
                minimax(
                    moved,
                    currentdepth + 1,
                    maxdepth
                ).nodevalue;

            if(value > best_value){

                best_value = value;
                best_move = move;
            }

            continue;
        }

        /*
        Expectation over random spawns

        90% chance of 2
        10% chance of 4
        */

        double expected_value = 0.0;

        for(const auto& pos : empty){

            int r = pos.first;
            int c = pos.second;

            {
                board spawn2 = moved;

                spawn2.grid[r][c] = 2;

                double value2 =
                    minimax(
                        spawn2,
                        currentdepth + 1,
                        maxdepth
                    ).nodevalue;

                expected_value +=
                    0.9 * value2;
            }

            {
                board spawn4 = moved;

                spawn4.grid[r][c] = 4;

                double value4 =
                    minimax(
                        spawn4,
                        currentdepth + 1,
                        maxdepth
                    ).nodevalue;

                expected_value +=
                    0.1 * value4;
            }
        }

        expected_value /= empty.size();

        if(expected_value > best_value){

            best_value = expected_value;
            best_move = move;
        }
    }

    result.nodevalue = best_value;
    result.move = best_move;

    return result;
}