#include <iostream>
#include <string>
#include "engine.h"

using namespace std;

string readable(string expin){

    //Make sure that all )( is replaced with )*(
    string exp = expin;

    for(int i = 0; i < exp.length() - 1; i++){
        if(exp[i] == ')' && exp[i+1] == '('){
            exp = exp.substr(0,i+1) + "*" + exp.substr(i+1,exp.length() - i - 1);
        }
    }

    //If a number is followed by a non-number and non-operation, add * in between
    for(int i = 0; i < exp.length() - 1; i++){
        if(
            exp[i] == '0' ||
            exp[i] == '1' ||
            exp[i] == '2' ||
            exp[i] == '3' ||
            exp[i] == '4' ||
            exp[i] == '5' ||
            exp[i] == '6' ||
            exp[i] == '7' ||
            exp[i] == '8' ||
            exp[i] == '9'
        ){
            if(!(
                exp[i+1] == '0' ||
                exp[i+1] == '1' ||
                exp[i+1] == '2' ||
                exp[i+1] == '3' ||
                exp[i+1] == '4' ||
                exp[i+1] == '5' ||
                exp[i+1] == '6' ||
                exp[i+1] == '7' ||
                exp[i+1] == '8' ||
                exp[i+1] == '9' ||

                exp[i+1] == '+' ||
                exp[i+1] == '-' ||
                exp[i+1] == '*' ||
                exp[i+1] == '/' ||
                exp[i+1] == '^' ||
                exp[i+1] == ')'
                )
            ){
                exp = exp.substr(0,i+1) + "*" + exp.substr(i+1,exp.length() - i - 1);
            }
        }
    }

    //If a parentheses is closed without a following operation, assume multiplication
    for(int i = 0; i < exp.length() - 1; i++){
        if(exp[i] == ')'){
            if(!(
                exp[i+1] == '+' ||
                exp[i+1] == '-' ||
                exp[i+1] == '*' ||
                exp[i+1] == '/' ||
                exp[i+1] == '^' ||
                exp[i+1] == ')'
                )
            ){
                exp = exp.substr(0,i+1) + "*" + exp.substr(i+1,exp.length() - i - 1);
            }
        }
    }

    //If entire expression is wrapped in parentheses, return without parentheses
    if(exp[0] == '('){
        if(exp[exp.length() - 1] == ')'){
            int openparentheses = -1;
            bool flag = false;
            for (int i = exp.length() - 2; i > 0; i--){
                //Only dissolve endpoint parentheses if they are linked, e.g.:
                //(5x/4)         LINKED
                //(x+1)/(2x+5)   NOT LINKED
                if(exp[i] == '('){
                    openparentheses++;
                }
                if(exp[i] == ')'){
                    openparentheses--;
                }

                if(openparentheses == 0){
                    flag = true;
                    break;
                }
            }
            if(!flag){
                return readable(exp.substr(1,exp.length()-2));
            }
        }
    }

    //Search for +/- since it is lowest in order of operations
    //Searched from end of expression to start to find the last operation
    //Example of forward searching failing: 7*8*9-1*2*3-4*5*6
    int operationindex = -1;
    int openparentheses = 0;
    for (int i = exp.length() - 1; i >= 0; i--){
        //Count open parentheses
        if(exp[i] == '('){
            openparentheses++;
        }
        
        if(exp[i] == ')'){
            openparentheses--;
        }
        
        if((exp[i] == '+' || exp[i] == '-') && openparentheses == 0){
            operationindex = i;
            break;
        }
    }
    if(operationindex != -1){ //If there is a +/- outside of parentheses
        if(exp[operationindex] == '+'){
            return "add(" + readable(exp.substr(0,operationindex)) + "," + readable(exp.substr(operationindex + 1, exp.length() - operationindex - 1)) + ")";
        } else if(exp[operationindex] == '-'){
            return "sub(" + readable(exp.substr(0,operationindex)) + "," + readable(exp.substr(operationindex + 1, exp.length() - operationindex - 1)) + ")";
        }
    }


    //If there is no +/- outside of parentheses, search for * or /
    //Similar argument can be made for backward searching
    operationindex = -1;
    openparentheses = 0;
    for (int i = exp.length() - 1; i >= 0; i--){
        //Count parentheses
        if(exp[i] == '('){
            openparentheses++;
        }
        
        if(exp[i] == ')'){
            openparentheses--;
        }
        
        if((exp[i] == '*' || exp[i] == '/') && openparentheses == 0){
            operationindex = i;
            break;
        }
    }
    if(operationindex != -1){ //If there is a * or / outside of parentheses
        if(exp[operationindex] == '*'){
            return "mul(" + readable(exp.substr(0,operationindex)) + "," + readable(exp.substr(operationindex + 1, exp.length() - operationindex - 1)) + ")";
        } else if(exp[operationindex] == '/'){
            return "div(" + readable(exp.substr(0,operationindex)) + "," + readable(exp.substr(operationindex + 1, exp.length() - operationindex - 1)) + ")";
        }
    }

    //If no 4 operations were found, search for ^ (power)
    operationindex = -1;
    openparentheses = 0;
    for (int i = exp.length() - 1; i >= 0; i--){
        //Count open parentheses
        if(exp[i] == '('){
            openparentheses++;
        }
        
        if(exp[i] == ')'){
            openparentheses--;
        }
        
        if((exp[i] == '^') && openparentheses == 0){
            operationindex = i;
            break;
        }
    }
    if(operationindex != -1){ //If there is a ^ outside of parentheses
        return "pow(" + readable(exp.substr(0,operationindex)) + "," + readable(exp.substr(operationindex + 1, exp.length() - operationindex - 1)) + ")";
    }

    return exp;

    /*
    //Case where the whole expression is not wrapped around parentheses
    //and no +, -, *, /, ^ were found outside of parentheses
    //can only mean we are looking at a term, which is a coefficient times a series of variables
    //or a singular number
    // IMPORTANT: THE FOLLOWING CODE ASSUMES THAT VARIABLES ARE SINGLE LETTERS.

    bool isNum = (
           exp[exp.length()-1] == '0' 
        || exp[exp.length()-1] == '1' 
        || exp[exp.length()-1] == '2' 
        || exp[exp.length()-1] == '3'
        || exp[exp.length()-1] == '4'
        || exp[exp.length()-1] == '5'
        || exp[exp.length()-1] == '6'
        || exp[exp.length()-1] == '7'
        || exp[exp.length()-1] == '8'
        || exp[exp.length()-1] == '9'
    );

    if(isNum){
        return exp;
    } else {
        int index = -1;
        for(int i = 0; i < exp.length(); i++){
            isNum = (    
                   exp[i] == '0' 
                || exp[i] == '1' 
                || exp[i] == '2' 
                || exp[i] == '3'
                || exp[i] == '4'
                || exp[i] == '5'
                || exp[i] == '6'
                || exp[i] == '7'
                || exp[i] == '8'
                || exp[i] == '9'
                || exp[i] == '.'
            );

            if(!isNum){
                index = i;
                break;
            }
        }
        string construct = exp.substr(0,index);
        if(index == 0){
            construct = "1";
        }

        while(index < exp.length()){
            construct = (string("mul(") + exp[index]) + "," + construct + ")"; 
            index++;
        }
        return construct;

    }
    */

}
