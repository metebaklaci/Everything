#include <bits/stdc++.h>

using namespace std;

int main(){
    double x_init = 0;
    double v_init = 0; // m/s
    double c_d = 0.75; // dimensionless
    double air_density = 1.223; // kg/m^3
    double area = 3.141592 * 0.35 * 0.35; // m^2
    double mass = 0.; // kg
    double g = -9.80665; // m/s^2

    double x = x_init; // m
    double v = v_init; // m/s
    double a; // m/s^2
    double f_d = 0; // N
    double f_g; // N

    double time = 0; // s
    double dt = 0.0001; // s

    double target_time = 4; // s

    for(int i = 0; i*dt < target_time; ++i){

        f_d = c_d * air_density * v * v * 0.5 * area;
        f_g = g * mass;

        a = (f_d + f_g) / mass;

        v += a * dt;

        x += v * dt;

        time += dt;

        if(2*((i+1) * dt) == floor(2*((i+1) * dt))) cout << "Time: " << time << "   Displacement: " << x << "   Velocity: " << v << "\n";
    }

}