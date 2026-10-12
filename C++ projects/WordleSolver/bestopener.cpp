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

    cout << "Done converting to Word objects. \nFinding best opener:\n";
    
    Word best = bestguess(guesses, answers);
    cout << "Found.\nBest word:\n" << wordToString(best);

    cout << "\n";
}