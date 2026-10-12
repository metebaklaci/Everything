#include <bits/stdc++.h>

using namespace std;

//d2y/dx2 = (-k/m)x

double k = 10; // N/m
double m = 1; //kg

//d2y/dx2 = g + a(dx/dt) + b(dx/dt)^2

double a = 2;
double b = 1;
double g = -8;

double dxdt(double x, double v, double t){
    return v;
}

double dvdt(double x, double v, double t){
    return g + a*v + b*v*v;
}

int main(){

    cout << time(0) << "\n";

    cout << fixed << setprecision(20);

    //initial values
    double x_0 = 1; //m
    double v_0 = 0; //m/s

    //step size in s
    double h = (((1/128)/128)/128)/128; //2^(-28), 4 nanosecond precision

    //approximation range in s
    double bound_low = 0;
    double bound_high = 5;

    double prevx = x_0;
    double prevv = v_0;

    for(double t = bound_low; t <= bound_high; t += h){
        double k1x = dxdt(prevx, prevv, t);
        double k1v = dvdt(prevx, prevv, t);

        double k2x = dxdt(prevx + (h/2)*k1x, prevv + (h/2)*k1v, t + (h/2));
        double k2v = dvdt(prevx + (h/2)*k1x, prevv + (h/2)*k1v, t + (h/2));

        double k3x = dxdt(prevx + (h/2)*k2x, prevv + (h/2)*k2v, t + (h/2));
        double k3v = dvdt(prevx + (h/2)*k2x, prevv + (h/2)*k2v, t + (h/2));

        double k4x = dxdt(prevx + (h)*k3x, prevv + (h)*k3v, t + (h));
        double k4v = dvdt(prevx + (h)*k3x, prevv + (h)*k3v, t + (h));
        
        prevx += (h/6)*(k1x + 2*k2x + 2*k3x + k4x);
        prevv += (h/6)*(k1v + 2*k2v + 2*k3v + k4v);

        if(128*t == floor(128*t) || abs(prevx) < 0.000001) cout << "Time: " << t << "   Displacement: " << prevx << "   Velocity: " << prevv << "\n";
    }

    cout << time(0);

    return 0;
}