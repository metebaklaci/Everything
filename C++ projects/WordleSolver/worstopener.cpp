#include "solver.h"

using namespace std;

int main(){

    cout << "Reading files:\n";

    vector<string> guesses_t = readLines("accepted.txt");
    vector<string> answers_t = readLines("answers.txt");

    cout << "Done reading.\nConverting to Word objects:\n";

    vector<Word> guesses = {};
    vector<Word> answers = {};

    for(string guess : guesses_t){
        guesses.push_back(Word(guess));
    }

    for(string answer : answers_t){
        answers.push_back(Word(answer));
    }

    cout << "Done converting to Word objects. \nFinding WORST opener:\n";

    Word wguess = Word("*****");
    double worstinfo = 9999999999;
    int count = 0;
    for(Word guess : guesses){
        count++;
        double ent = entropy(guess, answers);
        if(ent < worstinfo){
            wguess = guess;
            worstinfo = ent;
        }
    }

    cout << "Worst opener: " << wordToString(wguess);

    cout << "\n";
}