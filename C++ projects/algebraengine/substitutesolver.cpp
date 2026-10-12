#include "engine.h"
#include <string>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

substitution createsub(char varin, double valin){
    substitution sub_;
    sub_.var = varin;
    sub_.val = valin;
    return sub_;
}

string sub(string exp_, vector<substitution> subs){
    string exp = exp_;
    string valtostr;
    for(int i = 0; i < subs.size(); i++){
        valtostr = to_string(subs[i].val);
        for(int j = 0; j < exp.length(); j++){
            if(exp[j] == subs[i].var){
                exp = exp.substr(0,j) + valtostr + exp.substr(j+1, exp.length() -j-1);
            }
        }
    }
    return exp;
}

string sub(string exp_, substitution subs){
    string exp = exp_;
    string valtostr;
    valtostr = to_string(subs.val);
    for(int j = 0; j < exp.length(); j++){
        if(exp[j] == subs.var){
            exp = exp.substr(0,j) + valtostr + exp.substr(j+1, exp.length() -j-1);
        }
    }
    return exp;
}

int comma(string& exp){
    int openpar = 0;
    for(int i = 0; i < exp.length(); i++){
        if(exp[i] == '('){
            openpar++;
        }
        if(exp[i] == ')'){
            openpar--;
        }
        if(exp[i] == ',' && openpar == 1){
            return i;
        }
    }
    return -1;
}

double calculate(string exp){

    if(exp[0] == 'a'){
        int index = comma(exp);
        return calculate(exp.substr(4,index-4)) + calculate(exp.substr(index+1, exp.length() - index - 2));
    } else if(exp[0] == 's'){
        int index = comma(exp);
        return calculate(exp.substr(4,index-4)) - calculate(exp.substr(index+1, exp.length() - index - 2));
    } else if(exp[0] == 'm'){
        int index = comma(exp);
        return calculate(exp.substr(4,index-4)) * calculate(exp.substr(index+1, exp.length() - index - 2));
    } else if(exp[0] == 'd'){
        int index = comma(exp);
        return calculate(exp.substr(4,index-4)) / calculate(exp.substr(index+1, exp.length() - index - 2));
    } else if(exp[0] == 'p'){
        int index = comma(exp);
        return pow(calculate(exp.substr(4,index-4)),calculate(exp.substr(index+1, exp.length() - index - 2)));
    } else {
        return stod(exp);
    }

}