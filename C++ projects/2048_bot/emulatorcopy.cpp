#include "bot2048copy.h"

using namespace std;

static mt19937 rng(random_device{}());

static inline void reverseLine(array<int,4>& line){

    swap(line[0], line[3]);
    swap(line[1], line[2]);
}

static bool processLine(
    array<int,4>& line,
    int& points
){

    array<int,4> original = line;

    vector<int> compressed;

    for(int v : line){

        if(v != 0)
            compressed.push_back(v);
    }

    vector<int> merged;

    for(int i = 0; i < compressed.size(); ++i){

        if(
            i + 1 < compressed.size()
            &&
            compressed[i] == compressed[i + 1]
        ){

            int val = compressed[i] * 2;

            merged.push_back(val);

            points += val;

            ++i;
        }
        else{
            merged.push_back(compressed[i]);
        }
    }

    line = {0,0,0,0};

    for(int i = 0; i < merged.size(); ++i){
        line[i] = merged[i];
    }

    return line != original;
}

void prettyprint(const board& b){

    cout << "\n\nPoints: " << b.points << "\n\n";

    for(int r = 0; r < 4; ++r){

        for(int c = 0; c < 4; ++c){

            if(b.grid[r][c] == 0)
                cout << ".";
            else
                cout << b.grid[r][c];

            int spaces = 6;

            if(b.grid[r][c] != 0){

                int digits = to_string(
                    b.grid[r][c]
                ).size();

                spaces -= digits;
            }

            for(int i = 0; i < spaces; ++i){
                cout << " ";
            }
        }

        cout << "\n\n";
    }
}

bool randomspawn(board& b){

    vector<pair<int,int>> empty;

    for(int r = 0; r < 4; ++r){

        for(int c = 0; c < 4; ++c){

            if(b.grid[r][c] == 0){
                empty.push_back({r,c});
            }
        }
    }

    if(empty.empty())
        return false;

    uniform_int_distribution<int> posdist(
        0,
        empty.size() - 1
    );

    pair<int,int> pos = empty[posdist(rng)];

    uniform_real_distribution<double> prob(0,1);

    b.grid[pos.first][pos.second] =
        (prob(rng) < 0.9) ? 2 : 4;

    return true;
}

bool canMove(const board& b){

    for(int r = 0; r < 4; ++r){

        for(int c = 0; c < 4; ++c){

            if(b.grid[r][c] == 0)
                return true;

            if(
                r < 3
                &&
                b.grid[r][c] == b.grid[r + 1][c]
            ){
                return true;
            }

            if(
                c < 3
                &&
                b.grid[r][c] == b.grid[r][c + 1]
            ){
                return true;
            }
        }
    }

    return false;
}

bool emulate(
    board& b,
    int move,
    bool spawn
){

    bool moved = false;

    for(int i = 0; i < 4; ++i){

        array<int,4> line;

        if(move == 0){

            for(int r = 0; r < 4; ++r){
                line[r] = b.grid[r][i];
            }
        }
        else if(move == 2){

            for(int r = 0; r < 4; ++r){
                line[r] = b.grid[r][i];
            }

            reverseLine(line);
        }
        else if(move == 3){

            for(int c = 0; c < 4; ++c){
                line[c] = b.grid[i][c];
            }
        }
        else if(move == 1){

            for(int c = 0; c < 4; ++c){
                line[c] = b.grid[i][c];
            }

            reverseLine(line);
        }
        else{
            return false;
        }

        bool changed =
            processLine(line, b.points);

        moved = moved || changed;

        if(move == 2 || move == 1){
            reverseLine(line);
        }

        if(move == 0 || move == 2){

            for(int r = 0; r < 4; ++r){
                b.grid[r][i] = line[r];
            }
        }
        else{

            for(int c = 0; c < 4; ++c){
                b.grid[i][c] = line[c];
            }
        }
    }

    if(moved && spawn){
        randomspawn(b);
    }

    return moved;
}