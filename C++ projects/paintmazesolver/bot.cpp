#include "bot.h"

using namespace std;

vector<char> solve(grid g, int break_condition){

    if (painted(g)) return g.move_sequence;

    queue<grid> breadth_search;

    breadth_search.push(g);

    int i = 0;

    while(i < break_condition){

        if (breadth_search.empty()) break;

        grid currentgrid = breadth_search.front();
        breadth_search.pop();

        //cout << "Depth: " << currentgrid.move_sequence.size() << "\n";

        //prettyprint(currentgrid);

        grid move_w = move(currentgrid, 'w');
        grid move_a = move(currentgrid, 'a');
        grid move_s = move(currentgrid, 's');
        grid move_d = move(currentgrid, 'd');

        if(painted(move_w)) return move_w.move_sequence;
        else if(painted(move_a)) return move_a.move_sequence;
        else if(painted(move_s)) return move_s.move_sequence;
        else if(painted(move_d)) return move_d.move_sequence;

        if(move_w.metadata == '1'){
            breadth_search.push(move_w);
        }

        if(move_a.metadata == '1'){
            breadth_search.push(move_a);
        }

        if(move_s.metadata == '1'){
            breadth_search.push(move_s);
        }

        if(move_d.metadata == '1'){
            breadth_search.push(move_d);
        }

        i++;
    }

    cout << "Insufficient break condition" << "\n";
    return {'f','a','i','l'};

}