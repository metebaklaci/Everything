#include "bot2048.h"

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

using namespace std;

void prettyprint(board& b){
    //vector<string> emojis = {"⬜","🍞","🟨","🟧","🟥","⬛","🟫","🟪","🟦","🟩","🏆"};
    cout << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
    cout << "Points: " << b.points << endl << endl;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(b.grid[i][j] == 0) cout << " ";
            else cout << b.grid[i][j];
            for(int a = 0; a < 4 - floor(log10(b.grid[i][j] + 1)); a++){
                cout << " ";
            }
        }
        cout << endl << endl;
    }
}

void moveboard(board& brd, int move, int rowcol){ //DO NOT USE IF NO MOVE IS POSSIBLE ON ROW/COL
    if (move == 0){

        int topmostzero = 0;
        while(brd.grid[topmostzero][rowcol] != 0){
            topmostzero++;
        }
        int targetitem = topmostzero;
        while(brd.grid[targetitem][rowcol] == 0){
            targetitem++;
        }
        swap(brd.grid[targetitem][rowcol], brd.grid[topmostzero][rowcol]);

    } else if (move == 1){

        int rightmostzero = 0;
        while(brd.grid[rowcol][rightmostzero] != 0){
            rightmostzero++;
        }
        int targetitem = rightmostzero;
        while(brd.grid[rowcol][targetitem] == 0){
            targetitem++;
        }
        swap(brd.grid[rowcol][targetitem], brd.grid[rowcol][rightmostzero]);

    } else if (move == 2){

        int downmostzero = 3;
        while(brd.grid[downmostzero][rowcol] != 0){
            downmostzero--;
        }
        int targetitem = downmostzero;
        while(brd.grid[targetitem][rowcol] == 0){
            targetitem--;
        }
        swap(brd.grid[targetitem][rowcol], brd.grid[downmostzero][rowcol]);

    } else if (move == 3){

        int leftmostzero = 3;
        while(brd.grid[rowcol][leftmostzero] != 0){
            leftmostzero--;
        }
        int targetitem = leftmostzero;
        while(brd.grid[rowcol][targetitem] == 0){
            targetitem--;
        }
        swap(brd.grid[rowcol][targetitem], brd.grid[rowcol][leftmostzero]);

    }
}

bool mergeboard(board& brd, int move, int rowcol){
    bool merged = false;
    if (move == 0){
        for(int row = 0; row < 3; row++){
            if(brd.grid[row][rowcol] == brd.grid[row+1][rowcol]){
                brd.points += brd.grid[row][rowcol]*2;
                brd.grid[row][rowcol] *= 2;
                brd.grid[row+1][rowcol] = 0;
                if (brd.grid[row][rowcol] != 0) merged = true;
            }
        }
    } else if (move == 1){
        for(int col = 0; col < 3; col++){
            if(brd.grid[rowcol][col] == brd.grid[rowcol][col+1]){
                brd.points += brd.grid[rowcol][col]*2;
                brd.grid[rowcol][col] *= 2;
                brd.grid[rowcol][col+1] = 0;
                if (brd.grid[rowcol][col] != 0) merged = true;
            }
        }
    } else if (move == 2){
        for(int row = 3; row > 0; row--){
            if(brd.grid[row][rowcol] == brd.grid[row-1][rowcol]){
                brd.points += brd.grid[row][rowcol]*2;
                brd.grid[row][rowcol] *= 2;
                brd.grid[row-1][rowcol] = 0;
                if (brd.grid[row][rowcol] != 0) merged = true;
            }
        }
    } else if (move == 3){
        for(int col = 3; col > 0; col--){
            if(brd.grid[rowcol][col] == brd.grid[rowcol][col-1]){
                brd.points += brd.grid[rowcol][col]*2;
                brd.grid[rowcol][col] *= 2;
                brd.grid[rowcol][col-1] = 0;
                if (brd.grid[rowcol][col] != 0) merged = true;
            }
        }
    }
    return merged;
}

board randomspawn(board brd, int randin){
    board b = brd;
    vector<int> rows = {};
    vector<int> cols = {};
    for(int row = 0; row < 4; row++){
        for(int col = 0; col < 4; col++){
            if(b.grid[row][col] == 0){
                rows.push_back(row);
                cols.push_back(col);
            }
        }
    }
    int random = randin%rows.size();
    int rowpick = rows[random];
    int colpick = cols[random];
    b.grid[rowpick][colpick] = 2;
    return b;
}

bool emulate(board& brd, int move, bool spawn){
    bool returnflag = false;
    if (move == 0){
        for(int col = 0; col < 4; col++){
            //By moving, then merging, then moving again, we achieve 2048-like behavior
            while(!(
            (brd.grid[0][col] == 0 && brd.grid[1][col] == 0 && brd.grid[2][col] == 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] == 0 && brd.grid[2][col] == 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] != 0 && brd.grid[2][col] == 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] != 0 && brd.grid[2][col] != 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] != 0 && brd.grid[2][col] != 0 && brd.grid[3][col] != 0)
            ))
            { //Move phase 1
                moveboard(brd, move, col);
                returnflag = true;
            }

            //Merge
            returnflag = mergeboard(brd, move, col) || returnflag;

            while(!(
            (brd.grid[0][col] == 0 && brd.grid[1][col] == 0 && brd.grid[2][col] == 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] == 0 && brd.grid[2][col] == 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] != 0 && brd.grid[2][col] == 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] != 0 && brd.grid[2][col] != 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] != 0 && brd.grid[2][col] != 0 && brd.grid[3][col] != 0)
            ))
            { //Move phase 2
                moveboard(brd, move, col);
                returnflag = true;
            }

        }
    } else if (move == 1){
        for(int row = 0; row < 4; row++){
            //By moving, then merging, then moving again, we achieve 2048-like behavior
            while(!(
            (brd.grid[row][0] == 0 && brd.grid[row][1] == 0 && brd.grid[row][2] == 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] == 0 && brd.grid[row][2] == 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] != 0 && brd.grid[row][2] == 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] != 0 && brd.grid[row][2] != 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] != 0 && brd.grid[row][2] != 0 && brd.grid[row][3] != 0)
            ))
            { //Move phase 1
                moveboard(brd, move, row);
                returnflag = true;
            }

            //Merge
            returnflag = mergeboard(brd, move, row) || returnflag;

            while(!(
            (brd.grid[row][0] == 0 && brd.grid[row][1] == 0 && brd.grid[row][2] == 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] == 0 && brd.grid[row][2] == 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] != 0 && brd.grid[row][2] == 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] != 0 && brd.grid[row][2] != 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] != 0 && brd.grid[row][2] != 0 && brd.grid[row][3] != 0)
            ))
            { //Move phase 2
                moveboard(brd, move, row);
                returnflag = true;
            }

        }
    } else if (move == 2){
        for(int col = 0; col < 4; col++){
            //By moving, then merging, then moving again, we achieve 2048-like behavior
            while(!(
            (brd.grid[0][col] == 0 && brd.grid[1][col] == 0 && brd.grid[2][col] == 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] == 0 && brd.grid[1][col] == 0 && brd.grid[2][col] == 0 && brd.grid[3][col] != 0) ||
            (brd.grid[0][col] == 0 && brd.grid[1][col] == 0 && brd.grid[2][col] != 0 && brd.grid[3][col] != 0) ||
            (brd.grid[0][col] == 0 && brd.grid[1][col] != 0 && brd.grid[2][col] != 0 && brd.grid[3][col] != 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] != 0 && brd.grid[2][col] != 0 && brd.grid[3][col] != 0)
            ))
            { //Move phase 1
                moveboard(brd, move, col);
                returnflag = true;
            }

            //Merge
            returnflag = mergeboard(brd, move, col) || returnflag;

            while(!(
            (brd.grid[0][col] == 0 && brd.grid[1][col] == 0 && brd.grid[2][col] == 0 && brd.grid[3][col] == 0) ||
            (brd.grid[0][col] == 0 && brd.grid[1][col] == 0 && brd.grid[2][col] == 0 && brd.grid[3][col] != 0) ||
            (brd.grid[0][col] == 0 && brd.grid[1][col] == 0 && brd.grid[2][col] != 0 && brd.grid[3][col] != 0) ||
            (brd.grid[0][col] == 0 && brd.grid[1][col] != 0 && brd.grid[2][col] != 0 && brd.grid[3][col] != 0) ||
            (brd.grid[0][col] != 0 && brd.grid[1][col] != 0 && brd.grid[2][col] != 0 && brd.grid[3][col] != 0)
            ))
            { //Move phase 2
                moveboard(brd, move, col);
                returnflag = true;
            }

        }
    } else if (move == 3){
        for(int row = 0; row < 4; row++){
            //By moving, then merging, then moving again, we achieve 2048-like behavior
            while(!(
            (brd.grid[row][0] == 0 && brd.grid[row][1] == 0 && brd.grid[row][2] == 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] == 0 && brd.grid[row][1] == 0 && brd.grid[row][2] == 0 && brd.grid[row][3] != 0) ||
            (brd.grid[row][0] == 0 && brd.grid[row][1] == 0 && brd.grid[row][2] != 0 && brd.grid[row][3] != 0) ||
            (brd.grid[row][0] == 0 && brd.grid[row][1] != 0 && brd.grid[row][2] != 0 && brd.grid[row][3] != 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] != 0 && brd.grid[row][2] != 0 && brd.grid[row][3] != 0)
            ))
            { //Move phase 1
                moveboard(brd, move, row);
                returnflag = true;
            }

            //Merge
            returnflag = mergeboard(brd, move, row) || returnflag;


            while(!(
            (brd.grid[row][0] == 0 && brd.grid[row][1] == 0 && brd.grid[row][2] == 0 && brd.grid[row][3] == 0) ||
            (brd.grid[row][0] == 0 && brd.grid[row][1] == 0 && brd.grid[row][2] == 0 && brd.grid[row][3] != 0) ||
            (brd.grid[row][0] == 0 && brd.grid[row][1] == 0 && brd.grid[row][2] != 0 && brd.grid[row][3] != 0) ||
            (brd.grid[row][0] == 0 && brd.grid[row][1] != 0 && brd.grid[row][2] != 0 && brd.grid[row][3] != 0) ||
            (brd.grid[row][0] != 0 && brd.grid[row][1] != 0 && brd.grid[row][2] != 0 && brd.grid[row][3] != 0)
            ))
            { //Move phase 2
                moveboard(brd, move, row);
                returnflag = true;
            }

        }
    }
    if(returnflag && spawn) {brd = randomspawn(brd, rand());}
    return returnflag;
}