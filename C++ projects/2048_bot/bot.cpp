#include "bot2048.h" 

using namespace std;

int main(){
    srand(time(0));
    board b;
    
    b.points = 0;
    b.grid[0] = {0,0,0,0};
    b.grid[1] = {0,0,0,0};
    b.grid[2] = {0,0,0,0};
    b.grid[3] = {0,0,0,0};
    b = randomspawn(b, rand());
    b = randomspawn(b, rand());
    
    
    /*
    b.points = 1228;
    b.grid[0] = {128,2,2,0};
    b.grid[1] = {2048,4,0,0};
    b.grid[2] = {8,128,2,0};
    b.grid[3] = {1024,2,16,8};
    */

    int input = 0;
    while(input != -1){
        prettyprint(b);

        int zerocount = 0;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                if(b.grid[i][j] == 0){
                    zerocount++;
                }
            }
        }

        int targetdepth;
        if(zerocount > 6) targetdepth = 3;
        else if (zerocount > 2) targetdepth = 4;
        else targetdepth = 5;

        minimaxreturn returned = minimax(b, 0, targetdepth);

        int bestmove = 0;
        if (returned.move == 0) bestmove = 8;
        else if (returned.move == 1) bestmove = 4;
        else if (returned.move == 2) bestmove = 2;
        else if (returned.move == 3) bestmove = 6;
        cout << endl << "Best move: " << bestmove << endl << returned.nodevalue;
        cout << endl << "Move: ";
        //cin >> input;
        input = bestmove;
        if(input == 8){
            emulate(b, 0, true);
        } else if(input == 4){
            emulate(b, 1, true);
        }  else if(input == 2){
            emulate(b, 2, true);
        }  else if(input == 6){
            emulate(b, 3, true);
        }
        this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    return 0;
}