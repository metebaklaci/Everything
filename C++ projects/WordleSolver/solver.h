#ifndef SOLVER_H

#define SOLVER_H

#include <bits/stdc++.h>

struct Constraint{

    const std::vector<char> alphabet = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    //alphabet.at(i) = i + 97

    std::vector<int> min_count;
    std::vector<int> max_count;
    std::vector<char> known; //'*' represents unknown
    std::vector<std::vector<bool>> potential_locations;

    Constraint(std::vector<int> mincounts, std::vector<int> maxcounts, std::vector<char> knowns, std::vector<std::vector<bool>> potentiallocations){
        
        if(
            mincounts.size() != 26 ||
            maxcounts.size() != 26 ||
            knowns.size() != 5 ||
            potentiallocations.size() != 26 ||
            potentiallocations.at(0).size() != 5
        ) throw std::runtime_error("Parameters for constructor of constraint do not fit size requirements");

        min_count = mincounts;
        max_count = maxcounts;
        known = knowns;
        potential_locations = potentiallocations;

    }

    Constraint(){ //Unconstrained constructor
        min_count = std::vector<int>(26, 0);
        max_count = std::vector<int>(26, 5);
        known = {'*','*','*','*','*'};
        potential_locations = std::vector<std::vector<bool>>(26, std::vector<bool>(5, true));
    }

};

struct Word{

    std::vector<char> letters = {'*', '*', '*', '*', '*'};
    
    Word(std::vector<char> lets){

        if(lets.size() != 5) throw std::runtime_error("Parameter for word constructor is not of length 5");

        letters = lets;

    }

    Word(std::string word){

        if(word.size() != 5) throw std::runtime_error("Parameter for word constructor is not of length 5");

        for(int i = 0; i < 5; ++i){
            letters.at(i) = word.at(i);
        }

    }

};

void printConstraint(Constraint& constraint){

    std::cout << "\nKnown letters:\n"; //TODO: prettyprint constraint
    for(int i = 0; i < 5; ++i){
        std::cout << constraint.known.at(i);
    }

    std::cout << "\n\nMaximum and minimum bounds:\nLetter:       ";
    for(int i = 0; i < 26; ++i){
        std::cout << constraint.alphabet.at(i) << " ";
    }
    std::cout << "Lower bounds: ";
    for(int i = 0; i < 26; ++i){
        std::cout << constraint.min_count.at(i) << " ";
    }
    std::cout << "Upper bounds: ";
    for(int i = 0; i < 26; ++i){
        std::cout << constraint.max_count.at(i) << " ";
    }

    std::cout << "\n\nPotential locations:\n";
    for(int i = 0; i < 26; ++i){
        std::cout << constraint.alphabet.at(i) << " ";
    }
    std::cout << "\n";
    for(int i = 0; i < 5; ++i){
        for(int j = 0; j < 26; ++j){
            std::cout << constraint.potential_locations.at(j).at(i) << " ";
        }
        std::cout << "\n";
    }

}

bool fitsconstraint(Word word, Constraint constraint){

    //Check known letters first as it is most easily deniable

    for(int i = 0; i < 5; ++i){
        if(constraint.known.at(i) == '*') continue;
        if(word.letters.at(i) != constraint.known.at(i)) return false;
    }

    //Next check lower and upper bounds

    for(int i = 0; i < 26; ++i){
        int count = 0;
        for(int j = 0; j < 5; ++j){
            if(constraint.alphabet.at(i) == word.letters.at(j)) count++;
        }
        if(count < constraint.min_count.at(i) || count > constraint.max_count.at(i)) return false;
    }

    //Next check potential locations. If a letter is somewhere it cannot potentially exist, reject it

    for(int i = 0; i < 26; ++i){
        for(int j = 0; j < 5; ++j){
            if(word.letters.at(j) == constraint.alphabet.at(i) && !constraint.potential_locations.at(i).at(j)){
                return false;
            }
        }
    }

    //If all constraints have passed, return true
    return true;

}

//numOfYellows may also include greens
void updateConstraintGrey(Constraint& constraint, char letter, int position, int numOfYellows){

    constraint.max_count.at(letter - 97) = numOfYellows;

    if(numOfYellows == 0) constraint.potential_locations.at(letter - 97) = {false, false, false, false, false};
    else{
        constraint.potential_locations.at(letter - 97).at(position) = false;
    }

}

void updateConstraintGreen(Constraint& constraint, char letter, int position){

    constraint.known.at(position) = letter;

    constraint.min_count.at(letter - 97) = 0;
    for(char let : constraint.known){
        if(let == letter) constraint.min_count.at(letter - 97)++;
    }

}


/*
 * @param constraint The constraint to be updated
 * @param letter The char-type letter that the revealed yellow tile is
 * @param position The position of the yellow tile
 * @param numOfYellow The number of yellow and green tiles with letter that were revealed in the guess (including this one)
 * @param is_definite Whether or not a grey tile of the same letter was revealed along with the yellow tile
 * 
 * */
void updateConstraintYellow(Constraint& constraint, char letter, int position, int numOfYellows, bool is_definite){

    //the position that letter is in cannot hold that letter
    //if numOfYellows is larger than the minimum count for the letter, set the minimum count of the letter to numOfYellows
    //no update to upper bound
    //no update to known letters
    constraint.potential_locations.at(letter - 97).at(position) = false;
    if(numOfYellows > constraint.min_count.at(letter - 97)) constraint.min_count.at(letter - 97) = numOfYellows;
    if(is_definite) constraint.max_count.at(letter - 97) = numOfYellows;

}

void potentialSaturatedResolve(Constraint& constraint){ //If the lower bound is equal to the number of potential spots, those spots must be full.
    for(int i = 0; i < 26; ++i){
        int count = 0;
        for(bool spot : constraint.potential_locations.at(i)){
            if(spot) count++;
        }
        if(constraint.min_count.at(i) == count){
            for(int j = 0; j < 5; ++j){
                if(constraint.potential_locations.at(i).at(j)){
                    constraint.known.at(j) = constraint.alphabet.at(i);
                }
            }
        }
    }
}

void potentialUpperUpdate(Constraint& constraint){ //If there are less potential vacancies than the upper bound, set the upper bound to the number of potential vacancies
    for(int i = 0; i < 26; ++i){
        int count = 0;
        for(bool spot : constraint.potential_locations.at(i)){
            if(spot) count++;
        }
        if(count < constraint.max_count.at(i)) constraint.max_count.at(i) = count;
    }
}

void uncertaintyUpperResolve(Constraint& constraint){ //The upper bound for each letter can be the number of unknown locations + known places of that letter
    std::vector<int> counts(26, 0);
    int unknowns = 0;
    for(int i = 0; i < 5; ++i){
        if(constraint.known.at(i) == '*') unknowns++;
        else counts.at(constraint.known.at(i) - 97)++;
    }

    for(int i = 0; i < 26; ++i){
        constraint.max_count.at(i) = std::min(constraint.max_count.at(i), unknowns + counts.at(i));
    }
}

void resolveTautologies(Constraint& constraint){ //Call all update methods
    potentialUpperUpdate(constraint);
    potentialSaturatedResolve(constraint);
    uncertaintyUpperResolve(constraint);
}

/*
 * @param constraint The constraint to be updated
 * @param guess The guess for which clues are obtained
 * @param colors The colors of each tile opened (must be length 5). 'g' = grey, 'y' = yellow, 'r' = green
 * 
 * */
void constrainWithGuess(Constraint& constraint, Word guess, std::vector<char> colors){

    if(colors.size() != 5) throw std::runtime_error("Constrain with guess colors parameter had unfit size");

    for(int i = 0; i < 5; ++i){
        if(colors.at(i) == 'g'){
            int count = 0;
            for(int j = 0; j < 5; ++j){
                if(guess.letters.at(i) == guess.letters.at(j) && (colors.at(j) == 'y' || colors.at(j) == 'r')) count++;
            }
            updateConstraintGrey(constraint, guess.letters.at(i), i, count);
        }
        else if (colors.at(i) == 'r') updateConstraintGreen(constraint, guess.letters.at(i), i);

        else if(colors.at(i) == 'y'){
            int count = 0;
            bool grey = false;
            for(int j = 0; j < 5; ++j){
                if(guess.letters.at(i) == guess.letters.at(j) && (colors.at(j) == 'y' || colors.at(j) == 'r')) count++;
                if(guess.letters.at(i) == guess.letters.at(j) && colors.at(j) == 'g') grey = true;
            }
            updateConstraintYellow(constraint, guess.letters.at(i), i, count, grey);
        }
    }

    resolveTautologies(constraint);

}

std::vector<char> guessresult(Word& guess, Word& answer){
    std::vector<char> colors(5, 'g');

    std::vector<bool> answer_used(5, false);
    
    //greens
    for(int i = 0; i < 5; ++i){
        if(guess.letters.at(i) == answer.letters.at(i)){
            colors.at(i) = 'r';
            answer_used.at(i) = true;
        }
    }

    //yellows
    for(int i = 0; i < 5; ++i){
        if(colors.at(i) == 'r') continue;
        for(int j = 0; j < 5; ++j){
            if(answer_used.at(j)) continue;
            if(guess.letters.at(i) == answer.letters.at(j)){
                colors.at(i) = 'y';
                answer_used.at(j) = true;
                break;
            }
        }
    }

    //the rest are grey
    return colors;
}

double entropy(Word& guess, std::vector<Word>& possible_answers){

    if(possible_answers.size() == 1 && guess.letters == possible_answers.at(0).letters) return 9999999;
    double ent = 0;
    std::unordered_map<std::string, int> occurences = {}; //first: std::string of colors from guess, second: occurences of guess
    for(int i = 0; i < possible_answers.size(); ++i){
        std::vector<char> colors = guessresult(guess, possible_answers.at(i));
        std::string tile_colors = std::string(colors.begin(), colors.end());
        if(occurences.count(tile_colors)){
            occurences[tile_colors]++;
        } else {
            occurences[tile_colors] = 1;
        }
    }

    for(auto occurence : occurences){
        double probability = static_cast<double>(occurence.second) / possible_answers.size();
        ent -= probability * log(probability);
    }

    return ent;

}

Word bestguess(std::vector<Word>& guesses, std::vector<Word>& answers){

    if(guesses.empty()) throw std::runtime_error("bestguess called with empty guess list");

    Word bguess = Word("*****");
    double bestinfo = -9999999999;
    int count = 0;
    for(Word guess : guesses){
        count++;
        double ent = entropy(guess, answers);
        if(ent > bestinfo){
            bguess = guess;
            bestinfo = ent;
        }
    }
    return bguess;
}

std::vector<std::string> readLines(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;

    while (getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::vector<Word> constrainedSubset(Constraint& constraint, std::vector<Word> list){

    std::vector<Word> returner = {};

    for(int i = 0; i < list.size(); ++i){
        if(fitsconstraint(list.at(i), constraint)) returner.push_back(list.at(i));
    }

    return returner;

}

std::string wordToString(Word& word){
    std::string develop = "";
    for(int i = 0; i < 5; ++i){
        develop += word.letters.at(i);
    }

    return develop;
}

#endif