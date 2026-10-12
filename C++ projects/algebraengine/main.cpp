#include "engine.h"
#include <string>
#include <iostream>
#include <iomanip>

int main(){

    std::cout << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10);

    while(true){

        std::string expin;
        std::cout << "Expression to find zeroes: ";
        std::cin >> expin;

        std::string expout;
        expout = readable(expin);
        //std::cout << expout << std::endl;

        char var;
        std::cout << "Abscissa variable: ";
        std::cin >> var;

        int its;
        std::cout << "Number of iterations: ";
        std::cin >> its;

        double dx;
        std::cout << "dx in numerical derivative calculation: ";
        std::cin >> dx;

        double initial;
        std::cout << "Initial search point for Newton's method: ";
        std::cin >> initial;

        std::cout << "Zero: " << zeronewton(expout, var, its, dx, initial) << std::endl << std::endl;
    }
    return 0;
}