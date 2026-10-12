#include "gridmanager.h"

using namespace std;

int main(){
    grid g;

    g.map = {
        {'#', '#', '#', '#', '#', '#', '#', '#'},
        {'#', 'O', 'O', 'O', 'O', 'O', '#', '#'},
        {'#', 'O', 'O', '#', 'O', 'O', '#', '#'},
        {'#', 'O', 'O', 'O', 'O', 'O', 'O', '#'},
        {'#', 'O', 'O', 'O', 'O', 'O', 'O', '#'},
        {'#', 'O', 'O', 'O', 'O', '#', 'O', '#'},
        {'#', '#', 'O', 'O', 'O', 'O', 'O', '#'},
        {'#', 'O', 'O', 'O', 'O', 'O', 'O', '#'},
        {'#', 'P', 'O', 'O', 'O', 'O', '#', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#'},
    };
    g.player_x = 1; g.player_y = 8;

    prettyprint(g);

    char mov = '_';
    while(mov != 'x'){
        cin >> mov;
        if(mov == 'w' || mov == 'a' || mov == 's' || mov == 'd'){
            g = move(g, mov);
            prettyprint(g);
        }
    }
}