#ifndef ENGINE_H

#define ENGINE_H

#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <iomanip>

struct substitution{
    char var = 'x';
    long double val = 0;
};

std::string readable(std::string expin);

substitution createsub(char var, double val);

std::string sub(std::string exp_, std::vector<substitution> subs);
std::string sub(std::string exp_, substitution subs);

int comma(std::string& exp);

double calculate(std::string exp);

long double zeronewton(std::string& exp, const char var, const int iterations, const double dx, const double seed);

#endif