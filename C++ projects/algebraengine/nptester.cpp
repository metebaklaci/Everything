#include "engine.h"

int main(){
    std::string expin;
    std::cout << "Expression to process: ";
    std::cin >> expin;

    std::cout << "\n" << readable(expin);
}