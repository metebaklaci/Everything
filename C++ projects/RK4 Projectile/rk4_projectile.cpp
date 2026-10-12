#include <bits/stdc++.h>
#include <quadmath.h>

using namespace std;

__float128 g = 9.801q;

//__float128 eta = 0.001002q;
__float128 eta = 0.0q;

__float128 c_D = 0.47q;
//__float128 rho = 1000.0q;
__float128 rho = 1.225q;


__float128 rho_object = 7800.0q;
__float128 m = 10.0q;
__float128 r = 0.1q;

const __float128 PI = 3.14159265358979323846264338327950288419716939937510582q;

__float128 dxdt(__float128 x, __float128 y, __float128 dxdt, __float128 dydt, __float128 t){
    return dxdt;
}

__float128 dydt(__float128 x, __float128 y, __float128 dxdt, __float128 dydt, __float128 t){
    return dydt;
}

__float128 d2xdt2(__float128 x, __float128 y, __float128 dxdt, __float128 dydt, __float128 t){
    __float128 k = sqrtq(dxdt*dxdt + dydt*dydt);
    return -(6*PI*r*eta*dxdt + k*dxdt*c_D*rho*PI*r*r*0.5q)/m;
}

__float128 d2ydt2(__float128 x, __float128 y, __float128 dxdt, __float128 dydt, __float128 t){
    __float128 k = sqrtq(dxdt*dxdt + dydt*dydt);
    return -(6*PI*r*eta*dydt + k*dydt*c_D*rho*PI*r*r*0.5q)/m - g + g*((rho/rho_object)-1.0q);
}



int main(){

    cout << time(0) << "\n";

    cout << fixed << setprecision(10);

    //initial values
    __float128 x_0 = 0; //m
    __float128 y_0 = 0; //m
    __float128 theta = (42.75q) * (PI/180.0q); //rad
    __float128 v_0 = 100;
    __float128 vx_0 = v_0*cosq(theta); //m/s
    __float128 vy_0 = v_0*sinq(theta); //m/s

    //step size in s
    __float128 h = (1.0q/128.0q)/128.0q; //2^-14

    //approximation range in s
    __float128 bound_low = 0;
    __float128 bound_high = 10;

    __float128 prevx = x_0;
    __float128 prevy = y_0;
    __float128 prevvx = vx_0;
    __float128 prevvy = vy_0;

    int iteration = 0;

    for(__float128 t = bound_low; t <= bound_high; t += h){

        if(prevy < 0){ 
            cout << "Time: " << static_cast<double>(t) << "   Displacement: (" << static_cast<double>(prevx) << ", " << static_cast<double>(prevy) << ")   Velocity: (" << static_cast<double>(prevvx) << ", " << static_cast<double>(prevvy) << ")\n";
            return 0;
        }

        if(iteration % (128*16) == 0) cout << "Time: " << static_cast<double>(t) << "   Displacement: (" << static_cast<double>(prevx) << ", " << static_cast<double>(prevy) << ")   Velocity: (" << static_cast<double>(prevvx) << ", " << static_cast<double>(prevvy) << ")\n";

        __float128 k1x = dxdt(prevx, prevy, prevvx, prevvy, t);
        __float128 k1y = dydt(prevx, prevy, prevvx, prevvy, t);
        __float128 k1vx = d2xdt2(prevx, prevy, prevvx, prevvy, t);
        __float128 k1vy = d2ydt2(prevx, prevy, prevvx, prevvy, t);

        __float128 k2x = dxdt(prevx + (h/2)*k1x, prevy + (h/2)*k1y, prevvx + (h/2)*k1vx, prevvy + (h/2)*k1vy, t + (h/2));
        __float128 k2y = dydt(prevx + (h/2)*k1x, prevy + (h/2)*k1y, prevvx + (h/2)*k1vx, prevvy + (h/2)*k1vy, t + (h/2));
        __float128 k2vx = d2xdt2(prevx + (h/2)*k1x, prevy + (h/2)*k1y, prevvx + (h/2)*k1vx, prevvy + (h/2)*k1vy, t + (h/2));
        __float128 k2vy = d2ydt2(prevx + (h/2)*k1x, prevy + (h/2)*k1y, prevvx + (h/2)*k1vx, prevvy + (h/2)*k1vy, t + (h/2));

        __float128 k3x = dxdt(prevx + (h/2)*k2x, prevy + (h/2)*k2y, prevvx + (h/2)*k2vx, prevvy + (h/2)*k2vy, t + (h/2));
        __float128 k3y = dydt(prevx + (h/2)*k2x, prevy + (h/2)*k2y, prevvx + (h/2)*k2vx, prevvy + (h/2)*k2vy, t + (h/2));
        __float128 k3vx = d2xdt2(prevx + (h/2)*k2x, prevy + (h/2)*k2y, prevvx + (h/2)*k2vx, prevvy + (h/2)*k2vy, t + (h/2));
        __float128 k3vy = d2ydt2(prevx + (h/2)*k2x, prevy + (h/2)*k2y, prevvx + (h/2)*k2vx, prevvy + (h/2)*k2vy, t + (h/2));

        __float128 k4x = dxdt(prevx + (h)*k3x, prevy + (h)*k3y, prevvx + (h)*k3vx, prevvy + (h)*k3vy, t + (h));
        __float128 k4y = dydt(prevx + (h)*k3x, prevy + (h)*k3y, prevvx + (h)*k3vx, prevvy + (h)*k3vy, t + (h));
        __float128 k4vx = d2xdt2(prevx + (h)*k3x, prevy + (h)*k3y, prevvx + (h)*k3vx, prevvy + (h)*k3vy, t + (h));
        __float128 k4vy = d2ydt2(prevx + (h)*k3x, prevy + (h)*k3y, prevvx + (h)*k3vx, prevvy + (h)*k3vy, t + (h));
        
        prevx += (h/6)*(k1x + 2*k2x + 2*k3x + k4x);
        prevy += (h/6)*(k1y + 2*k2y + 2*k3y + k4y);
        prevvx += (h/6)*(k1vx + 2*k2vx + 2*k3vx + k4vx);
        prevvy += (h/6)*(k1vy + 2*k2vy + 2*k3vy + k4vy);

        iteration++;
    }

    cout << time(0);

    return 0;
}