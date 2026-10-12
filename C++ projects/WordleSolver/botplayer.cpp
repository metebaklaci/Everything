#include "solver.h"

using namespace std;

int main(){

    Constraint constraint = Constraint();

    cout << "Reading files:\n";

    vector<string> guesses_t = readLines("accepted.txt");
    vector<string> answers_t = readLines("answers.txt");

    cout << "Done reading.\nConverting to word objects:\n";

    vector<Word> guesses = {};
    vector<Word> answers = {};

    for(string guess : guesses_t){
        guesses.push_back(Word(guess));
    }

    for(string answer : answers_t){
        answers.push_back(Word(answer));
    }

    cout << "Done converting.\n";

    for(int i = 0; i < 6; ++i){

        if(answers.size() == 0){
            cout << "\n\nThe input provided was wrong. No words exist that satisfy the constraints entered.\n\n";
            return -1;
        }

        cout << "There are " << answers.size() << " possible words.\n";

        cout << "For move " << i + 1 << ", calculating best move:\n";

        if(i == 0){
            cout << "The best guess is: " << "tarse" << "\nFinding best move that may be correct:\n";
            cout << "The best probable guess is: " << "raise" << "\nWhat are you guessing? (Enter in lowercase)\n";
        } else {
            Word bguess = bestguess(guesses, answers);

            cout << "The best guess is: " << wordToString(bguess) << "\nFinding best move that may be correct:\n";

            Word possibleguess = bestguess(answers, answers);

            cout << "The best probable guess is: " << wordToString(possibleguess) << "\nWhat are you guessing? (Enter in lowercase)\n";
        }

        string userguess;
        cin >> userguess;

        cout << "What is the result? Enter as a sequence of letters ('r' = green, 'y' = yellow, 'g' = grey)\n";
        string result;
        cin >> result;

        if(result == "rrrrr"){
            cout << "Victory - found in " << i + 1 << " guesses.";
            return 0;
        }

        cout << "Constraining search space:\n";

        vector<char> colors = {};
        for(int i = 0; i < 5; ++i){
            colors.push_back(result[i]);
        }

        constrainWithGuess(constraint, Word(userguess), colors);

        answers = constrainedSubset(constraint, answers);

    }

    cout << "Failed to find word in 6 guesses. The possible words from here are:\n";
    for(Word possibility : answers){
        cout << wordToString(possibility) << "\n";
    }

}