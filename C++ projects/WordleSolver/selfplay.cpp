#include "solver.h"

using namespace std;

int main(){

    cout << "Reading files:\n";

    vector<string> guesses_t = readLines("accepted.txt");
    vector<string> answers_t = readLines("answers.txt");

    cout << "Done reading.\nConverting to Word objects:\n";

    vector<Word> guesses_ = {};
    vector<Word> answers_ = {};

    for(string guess : guesses_t){
        guesses_.push_back(Word(guess));
    }

    for(string answer : answers_t){
        answers_.push_back(Word(answer));
    }

    cout << "Done converting.\n";


    const int NUMBEROFTESTS = 100;
    const int POSSIBLEGUESSTHRESHOLD = 300; //higher number -> more aggressive, also faster

    vector<int> results(7, 0);

    int total = 0;

    int dnf = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(0, answers_.size() - 1);


    for(int test = 0; test < NUMBEROFTESTS; ++test){

        cout << "Test " << test << " ";

        Constraint constraint = Constraint();

        Word answer = answers_.at(distr(gen));

        vector<Word> guesses = guesses_;
        vector<Word> answers = answers_;

        int attempts = -1;

        for(int i = 0; i < 6; ++i){

            Word guess("*****");

            if(i == 0){
                guess = Word("tarse");
            } else {
                if(answers.size() > POSSIBLEGUESSTHRESHOLD) guess = bestguess(guesses, answers);
                else guess = bestguess(answers, answers);
            }

            vector<char> colors = guessresult(guess, answer);

            if(colors == vector<char>({'r','r','r','r','r'})){
                attempts = i + 1;
                break;
            }

            constrainWithGuess(constraint, Word(guess), colors);

            answers = constrainedSubset(constraint, answers);

            if(answers.empty()){
                cout << "No answers remain after guess "
                    << wordToString(guess)
                    << '\n';
                printConstraint(constraint);
                throw std::runtime_error("Impossible constraint");
            }

        }

        cout << "Done\n";

        if(attempts == -1) dnf++;
        else total += attempts;

        if(attempts == -1) results[0]++;
        else results[attempts]++;

    }

    cout << "\n\n\n\nFinal result:\nAverage guesses per solve: " << ((double)total) / (NUMBEROFTESTS - dnf) << "\nNumber of failed attempts: " << dnf;

    cout << "\nGuess count distribution:\nDNF: " << results[0] << " times";
    for(int i = 1; i < 7; i++){
        cout << "\n" << i <<" guesses: " << results[i] << " times";
    } 
}