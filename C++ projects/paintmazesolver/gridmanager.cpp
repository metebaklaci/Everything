#include "gridmanager.h"

using namespace std;

bool painted(grid g){
    for(int i = 0; i < g.map.size(); ++i){
        for(int j = 0; j < g.map[i].size(); ++j){
            if(g.map[i][j] == 'O') return false;
        }
    }
    return true;
}

/*
 * @param g Grid to play the move on
 * @param move 'w' for up, 'a' for left, 's' for down, 'd' for right
 */
grid move(grid g, char move){
    grid returngrid = g;
    if(move == 'w'){
        if(g.map[g.player_y - 1][g.player_x] == '#'){
            returngrid.metadata = '0';
            return returngrid;
        }
        while(returngrid.map[returngrid.player_y - 1][returngrid.player_x] != '#'){
            returngrid.map[returngrid.player_y - 1][returngrid.player_x] = 'P';
            returngrid.player_y--;
        }
        returngrid.metadata = '1';
        returngrid.move_sequence.push_back('w');
        return returngrid;
    } 
    
    else if(move == 'a'){
        if(g.map[g.player_y][g.player_x - 1] == '#'){
            returngrid.metadata = '0';
            return returngrid;
        }
        while(returngrid.map[returngrid.player_y][returngrid.player_x - 1] != '#'){
            returngrid.map[returngrid.player_y][returngrid.player_x - 1] = 'P';
            returngrid.player_x--;
        }
        returngrid.metadata = '1';
        returngrid.move_sequence.push_back('a');
        return returngrid;
    } 
    
    else if(move == 's'){
        if(g.map[g.player_y + 1][g.player_x] == '#'){
            returngrid.metadata = '0';
            return returngrid;
        }
        while(returngrid.map[returngrid.player_y + 1][returngrid.player_x] != '#'){
            returngrid.map[returngrid.player_y + 1][returngrid.player_x] = 'P';
            returngrid.player_y++;
        }
        returngrid.metadata = '1';
        returngrid.move_sequence.push_back('s');
        return returngrid;
    } 
    
    else if(move == 'd'){
        if(g.map[g.player_y][g.player_x + 1] == '#'){
            returngrid.metadata = '0';
            return returngrid;
        }
        while(returngrid.map[returngrid.player_y][returngrid.player_x + 1] != '#'){
            returngrid.map[returngrid.player_y][returngrid.player_x + 1] = 'P';
            returngrid.player_x++;
        }
        returngrid.metadata = '1';
        returngrid.move_sequence.push_back('d');
        return returngrid;
    }

    throw runtime_error("Invalid move");
}

void prettyprint(grid g){
    cout << "\n" << "\n";
    for(int i = 0; i < g.map.size(); ++i){
        for(int j = 0; j < g.map[i].size(); ++j){
            if(i == g.player_y && j == g.player_x) cout << "@ ";
            else cout << g.map[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}