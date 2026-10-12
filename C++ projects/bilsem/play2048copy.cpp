#include "bot2048copy.h"

using namespace std;

int main(){

    board b;

    randomspawn(b);
    randomspawn(b);

    while(true){

        prettyprint(b);

        cout << "\nEvaluation: "
             << eval(b)
             << "\n";

        if(!canMove(b)){

            cout << "\nGame over.\n";
            break;
        }

        cout
        << "\nMove "
        << "(8=up, 6=right, 2=down, 4=left, -1=quit): ";

        int input;
        cin >> input;

        if(input == -1)
            break;

        bool moved = false;

        if(input == 8){
            moved = emulate(b, 0, true);
        }
        else if(input == 6){
            moved = emulate(b, 1, true);
        }
        else if(input == 2){
            moved = emulate(b, 2, true);
        }
        else if(input == 4){
            moved = emulate(b, 3, true);
        }

        if(!moved){

            cout
            << "\nInvalid move.\n";

            this_thread::sleep_for(
                chrono::milliseconds(500)
            );
        }
    }

    return 0;
}