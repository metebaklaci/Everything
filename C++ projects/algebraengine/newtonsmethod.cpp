#include "engine.h"
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>

using namespace std;

long double zeronewton(string& exp, const char var, const int iterations, const double dx, const double seed){
    substitution subs = createsub(var, seed);
    double dy = 0;
    for(int i = 0; i < iterations; i++){
        double height = calculate(sub(exp,subs));
        subs.val += dx;
        dy = calculate(sub(exp,subs)) - height;
        subs.val -= dx;
        subs.val += -1 * height * dx / dy;
    }
    return subs.val;
}