#include <string>
#include <iostream>
//Recursive differentiator that inputs a computer-interpretable prefix syntax and outputs the derivative in computer-interpretable form

std::string differentiate(std::string expin){
    if(expin == "x"){
        return "1";
    } 
    
    else if(expin.substr(0,4) == "add("){
        std::string expression = expin.substr(4, expin.size() - 5);
        int openparentheses = 0;
        int i = 0;
        for(; expression[i] != ',' || openparentheses != 0; i++){
            if(expression[i] == '('){
                openparentheses++;
            } else if(expression[i] == ')'){
                openparentheses--;
            }
        }
        std::string arg1 = expression.substr(0,i);
        std::string arg2 = expression.substr(i+1);
        return "add(" + differentiate(arg1) + "," + differentiate(arg2) + ")";
    } 

    else if(expin.substr(0,4) == "sub("){
        std::string expression = expin.substr(4, expin.size() - 5);
        int openparentheses = 0;
        int i = 0;
        for(; expression[i] != ',' || openparentheses != 0; i++){
            if(expression[i] == '('){
                openparentheses++;
            } else if(expression[i] == ')'){
                openparentheses--;
            }
        }
        std::string arg1 = expression.substr(0,i);
        std::string arg2 = expression.substr(i+1);
        return "sub(" + differentiate(arg1) + "," + differentiate(arg2) + ")";
    } 

    else if(expin.substr(0,4) == "mul("){
        std::string expression = expin.substr(4, expin.size() - 5);
        int openparentheses = 0;
        int i = 0;
        for(; expression[i] != ',' || openparentheses != 0; i++){
            if(expression[i] == '('){
                openparentheses++;
            } else if(expression[i] == ')'){
                openparentheses--;
            }
        }
        std::string arg1 = expression.substr(0,i);
        std::string arg2 = expression.substr(i+1);
        return "add(mul(" + differentiate(arg1) + "," + arg2 + "),mul(" + arg1 + "," + differentiate(arg2) + "))";
    }

    else if(expin.substr(0,4) == "div("){
        std::string expression = expin.substr(4, expin.size() - 5);
        int openparentheses = 0;
        int i = 0;
        for(; expression[i] != ',' || openparentheses != 0; i++){
            if(expression[i] == '('){
                openparentheses++;
            } else if(expression[i] == ')'){
                openparentheses--;
            }
        }
        std::string arg1 = expression.substr(0,i);
        std::string arg2 = expression.substr(i+1);
        return "div(sub(mul(" + differentiate(arg1) + "," + arg2 + "),mul(" + arg1 + "," + differentiate(arg2) + ")),mult(" + arg2 + "," + arg2 + "))";
    }

    else if(expin.substr(0,4) == "pow("){
        std::string expression = expin.substr(4, expin.size() - 5);
        int openparentheses = 0;
        int i = 0;
        for(; expression[i] != ',' || openparentheses != 0; i++){
            if(expression[i] == '('){
                openparentheses++;
            } else if(expression[i] == ')'){
                openparentheses--;
            }
        }
        std::string arg1 = expression.substr(0,i);
        std::string arg2 = expression.substr(i+1);
        return "mul(pow(" + arg1 + "," + arg2 + "),add(mult(" + differentiate(arg2) + ",ln(" + arg1 + ")),div(mult(" + differentiate(arg1) + "," + arg2 + ")," + arg1 + ")))";
    }

    else if(expin.substr(0,4) == "sin("){
        std::string expression = expin.substr(4, expin.size() - 5);
        int openparentheses = 0;
        int i = 0;
        for(; expression[i] != ',' || openparentheses != 0; i++){
            if(expression[i] == '('){
                openparentheses++;
            } else if(expression[i] == ')'){
                openparentheses--;
            }
        }
        std::string arg1 = expression.substr(0,i);
        std::string arg2 = expression.substr(i+1);
        return "mul(pow(" + arg1 + "," + arg2 + "),add(mult(" + differentiate(arg2) + ",ln(" + arg1 + ")),div(mult(" + differentiate(arg1) + "," + arg2 + ")," + arg1 + ")))";
    }

    return expin;
}