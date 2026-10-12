#include "bot2048.h"

#include <vector>
#include <iostream>

using namespace std;

double eval(const board& b){
    double evaluation = 0;

    double zerocoef = 2;
    double adjacencycoef = 0.001; 
    double rimcoef = 0.05; 
    double pointscoef = 0.0015;


    //More zeroes = better DONE
    //Adjacent numbers next to each other = very better DONE
    //Bigger numbers on the rims = better, corners = even better DONE
    //Row/col with high tile full = better (maybe later)
    //Look for more strategies
    evaluation += b.points * pointscoef;
    
    int zerocount = 0;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(b.grid[i][j] == 0){
                zerocount++;
            }
        }
    }
    evaluation += zerocoef * zerocount;

    int adjacentcount = 0;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 4; j++){
            if((b.grid[i][j]+2) / (b.grid[i+1][j]+1) == 2){
                adjacentcount += pow(log2(b.grid[i][j]+1),2) + pow(log2(b.grid[i+1][j]+1),2);
            }
        }
    }
    for(int i = 1; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if((b.grid[i][j]+2) / (b.grid[i-1][j]+1) == 2){
                adjacentcount += pow(log2(b.grid[i][j]+1),2) + pow(log2(b.grid[i-1][j]+1),2);
            }
        }
    }
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 4; j++){
            if((b.grid[j][i]+2) / (b.grid[j][i+1]+1) == 2){
                adjacentcount += pow(log2(b.grid[j][i]+1),2) + pow(log2(b.grid[j][i+1]+1),2);
            }
        }
    }
    for(int i = 1; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if((b.grid[j][i]+2) / (b.grid[j][i-1]+1) == 2){
                adjacentcount += pow(log2(b.grid[j][i]+1),2) + pow(log2(b.grid[j][i-1]+1),2);
            }
        }
    }

    evaluation += adjacencycoef * adjacentcount;

    double rimvalue = 0;
    for(int i = 0; i < 4; i += 3){
        for(int j = 0; j < 4; j++){
            rimvalue += pow(log2(b.grid[i][j]+1),2);
            rimvalue += pow(log2(b.grid[j][i]+1),2);
        }
    }

    evaluation += rimcoef * rimvalue;








    return evaluation;
}