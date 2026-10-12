#include "bot2048copy.h"

#include <cmath>
#include <algorithm>

using namespace std;

static inline double tileValue(int tile){

    if(tile == 0)
        return 0.0;

    double x = log2(tile);

    return x * x;
}

double eval(const board& b){

    double evaluation = 0.0;

    constexpr double zerocoef      = 2.0;
    constexpr double adjacencycoef = 0.001;
    constexpr double rimcoef       = 0.05;
    constexpr double pointscoef    = 0.0015;

    /*
    Score contribution
    */

    evaluation += b.points * pointscoef;

    /*
    Empty tiles
    */

    int zero_count = 0;

    for(int r = 0; r < 4; ++r){

        for(int c = 0; c < 4; ++c){

            if(b.grid[r][c] == 0)
                zero_count++;
        }
    }

    evaluation += zerocoef * zero_count;

    /*
    Adjacent merge opportunities

    Reward equal neighbors.
    */

    double adjacency_value = 0.0;

    for(int r = 0; r < 4; ++r){

        for(int c = 0; c < 4; ++c){

            int current = b.grid[r][c];

            if(current == 0)
                continue;

            /*
            Right neighbor
            */

            if(c < 3){

                int right = b.grid[r][c + 1];

                if(current == right){

                    adjacency_value +=
                        tileValue(current)
                        +
                        tileValue(right);
                }
            }

            /*
            Down neighbor
            */

            if(r < 3){

                int down = b.grid[r + 1][c];

                if(current == down){

                    adjacency_value +=
                        tileValue(current)
                        +
                        tileValue(down);
                }
            }
        }
    }

    evaluation += adjacencycoef * adjacency_value;

    /*
    Rim preference

    Encourages large values near edges.
    */

    double rim_value = 0.0;

    for(int i = 0; i < 4; ++i){

        rim_value += tileValue(b.grid[0][i]);
        rim_value += tileValue(b.grid[3][i]);

        rim_value += tileValue(b.grid[i][0]);
        rim_value += tileValue(b.grid[i][3]);
    }

    evaluation += rimcoef * rim_value;

    return evaluation;
}