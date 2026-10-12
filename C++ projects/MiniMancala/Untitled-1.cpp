#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <ctime>
#include <cmath>
using namespace std;

//Structure definitions for return values of modules
struct reconstructreturn{
    vector<int> posit;
    bool last;
};

struct minimaxreturn{
    double nodevalue = 0;
    vector<int> move = {-1}; //default value is nonsensical on purpose
};

reconstructreturn reconstruct1ce(vector<int> pos, int move) {  //Returns updated board and eligibility to play again as a structure
    vector<int> position;
    position = pos;
    int piececount = position[move];
    bool lastpiece = false;
    int slot = move;
    if (move < 6) {
        position[move] = 0;
        while (piececount != 0) {
            slot++;
            slot%=14;
            if (slot != 13){
                piececount--;
                position[slot]++;
            }
        }
        if (slot == 6) {
            lastpiece = true;
        }
        if (position[slot] == 1 && slot < 6 && position[12-slot] != 0){
            position[6] += position[12-slot] + 1;
            position[12-slot] = 0;
            position[slot] = 0;
        }
    } else if (move > 6 && move != 13) {
        position[move] = 0;
        while (piececount != 0){
            slot++;
            slot%=14;
            if (slot != 6){
                piececount--;
                position[slot]++;

            }
        }
        if (slot == 13){
            lastpiece = true;
        }
        if (position[slot]==1 && slot > 6 && slot != 13 && position[12-slot] != 0){
            position[13] += position[12-slot] + 1;
            position[12-slot] = 0;
            position[slot] = 0;
        }
    }
    reconstructreturn returnvalue;
    returnvalue.posit = position;
    returnvalue.last = lastpiece;
    return returnvalue; //A struct (reconstructreturn) will be returned with new board and last position
}

vector<int> reconstruct(vector<int> pos, vector<int> move){  //Returns the reconstructed board after move is executed
    vector<int> position = pos;
    for(int i = 0; i < move.size(); i++){
        position = reconstruct1ce(position, move[i]).posit;
    }
    return position;
}

double evalboard(vector<int> position){

    double mancalaweight = 1; //Equal to pieces in our mancala minus pieces in opponents mancala
    double emptyspaceweight = 0.2; //Values come in a range from 5-10 from each team
    double piecesonboardweight = 0.6; //Pieces in opponent's lots minus pieces in our lots

    //TO-DO: Include pieces on board evaluation

    double eval = 0;

    eval += (position[6] - position[13]) * mancalaweight;

    int emptyspaces = 0;
    for (int i = 0; i<6; i++){
        if (position[i] == 0){
            emptyspaces += 10 - i;
        }
    }
    for (int i = 7; i<13; i++){
        if(position[i] == 0){
            emptyspaces -= 17 - i;
        }
    }
    eval += emptyspaces * emptyspaceweight;



    return eval;
}


vector< vector<int> > explore(vector<int> position, int team, vector<int> path){ //position = board to be explored; team is 1 if player 1, 2 if player 2; path = current exploration path that the function has recursed in. this path is explored.
    vector< vector<int> > moves = {};

    if (team == 1){
        for (int i = 0; i<6; i++){
            if (position[i] != 0){
                reconstructreturn report = reconstruct1ce(position, i);
                vector<int> pathnext = path;
                vector<int> added = {i};
                pathnext.insert(pathnext.end(), added.begin(), added.end());
                if (report.last){
                    vector< vector<int> > explored = explore(report.posit, team, pathnext);
                    moves.insert(moves.end(), explored.begin(), explored.end());
                } else {
                    moves.push_back(pathnext);
                }
            }
        }
        return moves;
    } else if (team == 2){
        for (int i = 7; i<13; i++){
            if (position[i] != 0){
                reconstructreturn report = reconstruct1ce(position, i);
                vector<int> pathnext = path;
                vector<int> added = {i};
                pathnext.insert(pathnext.end(), added.begin(), added.end());
                if (report.last){
                    vector< vector<int> > explored = explore(report.posit, team, pathnext);
                    moves.insert(moves.end(), explored.begin(), explored.end());
                } else {
                    moves.push_back(pathnext);
                }
            }
        }
        return moves;
    } else {
        return moves; //team will always be 1 or 2 so this is only to make sure there is a return in every path
    }
}

vector< vector<int> > possible(vector<int> position, int team){
    vector<int> empty = {};
    return explore(position, team, empty);
}

minimaxreturn minimax(vector<int> position, int currentdepth, int maxdepth){
    if (maxdepth == currentdepth){
        double evaluate = evalboard(position);
        minimaxreturn returner;
        returner.nodevalue = evaluate;
        return returner;
    } else {
        //Special case where if the game ends it should evaluate straight away
        vector< vector<int> > empty = {{}};
        if(possible(position, 1) == empty){
            minimaxreturn returner;
            returner.nodevalue = (48 - 2*position[13]) * 9999999;
            return returner;
        }
        
        vector< vector<int> > moves1 = possible(position, 1);
        double maxvalue = -99999999999;
        int maxindex = 0;
        for (int i = 0; i < moves1.size(); i++){
            vector<int> board1 = reconstruct(position, moves1[i]);
            
            //Newly added codeblock, return very large number if won, very small number if lost
            if(board1[7] + board1[8] + board1[9] + board1[10] + board1[11] + board1[12] == 0){
                minimaxreturn returner;
                returner.nodevalue = (48 - 2*position[6]) * -9999999;
                return returner;
            }

            vector< vector<int> > moves2 = possible(board1, 2);
            double minvalue = 99999999999;
            int minindex = 0;
            for (int j = 0; j < moves2.size(); j++){
                vector<int> board2 = reconstruct(board1, moves2[j]);
                double nodevalue = minimax(board2, currentdepth+1, maxdepth).nodevalue;
                if (nodevalue < minvalue){
                    minvalue = nodevalue;
                    minindex = j;
                }
            }
            if (minvalue > maxvalue){
                maxvalue = minvalue;
                maxindex = i;
            }
        }
        if (currentdepth != 0){
            minimaxreturn returner;
            returner.nodevalue = maxvalue;
            return returner;
        } else {
            minimaxreturn returner;
            returner.nodevalue = maxvalue;
            returner.move = moves1[maxindex];
            return returner;
        }
    }
}

minimaxreturn bestmove(vector<int> position, int maxdepth){
    return minimax(position, 0, maxdepth);
}

void prettyprint(vector<int>board){
    cout << "Board state: " << endl << endl;
        cout << "   ";
    for (int i = 12; i > 6; i--){
        cout << board[i] << "  ";
    }
    cout << endl << board[13] << "                    " << board[6] << endl;
    cout << "   ";
    for (int i = 0; i < 6; i++){
        cout << board[i] << "  ";
    }
    cout << endl << endl;
    
}

int main() {

    /*      The following code is to find a singular best move. Remove comment to activate
    vector<int> board = {4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0};
    //{4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0}   Starting board
    //{6, 5, 4, 3, 2, 1, 0, 4, 4, 4, 4, 4, 4, 0}   Test board to stress-test possible() function
    //{0, 0, 0, 3, 1, 1, 21, 0, 1, 0, 0, 0, 0, 21}  End-game test

    minimaxreturn move = bestmove(board, 3);

    for (int i = 0; i < move.move.size(); i++){
        cout << move.move[i] << " ";
    }
    cout << endl;
    //cout << move.nodevalue;
    */

    vector<int> board = {4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0};

    cout << "First player (1 if bot, 2 if human): " << endl;

    int first = 0;
    cin >> first;
    cout << endl;

    int moveinput;

    if(first == 2){
        prettyprint(board);
        cout << endl << endl << "Play your move: " << endl;
        cin >> moveinput;
        cout << endl;
        reconstructreturn newboard = reconstruct1ce(board, moveinput);
        board = newboard.posit;
        prettyprint(board);

        while (newboard.last){

            cout << "Play again: " << endl;
            cin >> moveinput;
            cout << endl;
            newboard = reconstruct1ce(board, moveinput);
            board = newboard.posit;
            prettyprint(board);
        }

        
    }
    minimaxreturn botmove;
    for(;;){

        //Bot's move starts

        cout << "Bot is thinking... " << endl;
        int thinkdepth = 3 + floor((board[6]+board[13])/17.0);
        botmove = bestmove(board, thinkdepth);
        cout << "Bot has played: " << endl;
        for (int i = 0; i < botmove.move.size(); i++){
            cout << botmove.move[i] << " ";
        }
        cout << endl;

        board = reconstruct(board, botmove.move);

        prettyprint(board);

        //Bot's move ends, human's move starts

        cout << endl << endl << "Play your move: " << endl;
        cin >> moveinput;
        cout << endl;
        reconstructreturn newboard = reconstruct1ce(board, moveinput);
        board = newboard.posit;
        prettyprint(board);

        while (newboard.last){

            cout << "Play again: " << endl;
            cin >> moveinput;
            cout << endl;
            newboard = reconstruct1ce(board, moveinput);
            board = newboard.posit;
            prettyprint(board);
        }
    }
}