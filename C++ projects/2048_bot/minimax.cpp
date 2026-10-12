#include "bot2048.h"
#include <iostream>

using namespace std;

minimaxreturn minimax(board b, int currentdepth, int maxdepth){
    minimaxreturn returner;
    if(currentdepth == maxdepth){
        returner.nodevalue = eval(b);
        return returner;
    } else {
        double max = INT_MIN;
        for(int i = 0; i < 4; i++){
            board brd = b;
            double averageval = INT_MIN;
            if(emulate(brd, i, false)){
                vector<int> rows = {};
                vector<int> cols = {};
                for(int row = 0; row < 4; row++){
                    for(int col = 0; col < 4; col++){
                        if(brd.grid[row][col] == 0){
                            rows.push_back(row);
                            cols.push_back(col);
                        }
                    }
                }
                if(rows.size() == 0){
                    averageval = -99999;
                } else {
                    averageval = 0;
                    for(int a = 0; a < rows.size(); a++){
                        board newbrd = randomspawn(brd, a);
                        minimaxreturn explored = minimax(newbrd, currentdepth+1, maxdepth);
                        averageval += explored.nodevalue;
                    }
                    averageval /= rows.size();
                }
            }
            
            if (averageval > max){
                max = averageval;
                returner.nodevalue = max;
                returner.move = i;
            }
        }
        return returner;
    }
}