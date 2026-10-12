#include "bot2048.h"

using namespace std;

int main(){
    srand(time(0));
    board b;
    b.grid[0] = {0,0,0,0};
    b.grid[1] = {0,0,0,0};
    b.grid[2] = {0,0,0,0};
    b.grid[3] = {0,0,0,0};
    randomspawn(b, rand());
    randomspawn(b, rand());

    int input = 0;
    while(input != -1){
        prettyprint(b);
        cout << endl << eval(b);
        cout << endl << "Move: ";
        cin >> input;
        if(input == 8){
            emulate(b, 0, true);
        } else if(input == 4){
            emulate(b, 1, true);
        }  else if(input == 2){
            emulate(b, 2, true);
        }  else if(input == 6){
            emulate(b, 3, true);
        }
    }
    
    return 0;
}