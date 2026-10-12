#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <windows.h>
#include "renderer.h"

using namespace std;

coordinate createCoord(double x_in, double y_in, double z_in){
    coordinate coord;
    coord.x = x_in;
    coord.y = y_in;
    coord.z = z_in;
    return coord;
}

coordinate add_c(const coordinate& a, const coordinate& b){
    coordinate sum;
    sum.x = a.x + b.x;
    sum.y = a.y + b.y;
    sum.z = a.z + b.z;
    return sum;
}

coordinate sub_c(const coordinate& a, const coordinate& b){
    coordinate dif;
    dif.x = a.x - b.x;
    dif.y = a.y - b.y;
    dif.z = a.z - b.z;
    return dif;
}

double dot(const coordinate& a, const coordinate& b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

coordinate cross(const coordinate& a, const coordinate& b){
    coordinate out;
    out.x = a.y * b.z - a.z * b.y;
    out.y = a.z * b.x - a.x * b.z;
    out.z = a.x * b.y - a.y * b.x;
    return out;
}

triangle createTri(double a, double b, double c, double d, double e, double f, double g, double h, double i){
    triangle t;
    t.a = createCoord(a,b,c);
    t.b = createCoord(d,e,f);
    t.c = createCoord(g,h,i);
    return t;
}

double signedVolume(const coordinate& a, const coordinate& b, const coordinate& c, const coordinate& d){
    return (1.0/6.0)*dot(cross(sub_c(b,a),sub_c(c,a)),sub_c(d,a));
}

bool intersect(const triangle& t, const line& l){
    return (signedVolume(l.q1,t.a,t.b,t.c) * signedVolume(l.q2,t.a,t.b,t.c) < 0) && (signedVolume(l.q1,l.q2,t.a,t.b) * signedVolume(l.q1,l.q2,t.b,t.c) > 0) && (signedVolume(l.q1,l.q2,t.a,t.b) * signedVolume(l.q1,l.q2,t.c,t.a) > 0);
}

void render(const world& w, double camera_z, double visibility, double x, double y, double dx, double dy){

    for(int i = 0; i < 50; i++){
        cout << endl;
    }


    for (double row = -1*y; row <= y; row += dy) {
        for (double col = -1*x; col <= x; col += dx) {
            line ray;
            ray.q1 = createCoord(col,row,camera_z);
            ray.q2 = createCoord(col,row,camera_z - visibility);
            string print = " ";
            bool flag = true;
            for (int i = 0; i < w.triangles.size() && flag; i++) {
                if(intersect(w.triangles[i],ray)){
                    flag = false;
                    print = "#";
                }
            }
            cout << print;
        }
        cout << endl;
    }
}

void roll(triangle& t, const coordinate& pivot, double theta){
    double c = cos(theta);
    double s = sin(theta);

    auto rotate = [&](coordinate& p) {
        double y = p.y - pivot.y;
        double z = p.z - pivot.z;

        double yr =  y * c - z * s;
        double zr =  y * s + z * c;

        p.y = yr + pivot.y;
        p.z = zr + pivot.z;
    };

    rotate(t.a);
    rotate(t.b);
    rotate(t.c);
}

void pitch(triangle& t, const coordinate& pivot, double theta){
    double c = cos(theta);
    double s = sin(theta);

    auto rotate = [&](coordinate& p) {
        double x = p.x - pivot.x;
        double z = p.z - pivot.z;

        double xr =  x * c + z * s;
        double zr = -x * s + z * c;

        p.x = xr + pivot.x;
        p.z = zr + pivot.z;
    };

    rotate(t.a);
    rotate(t.b);
    rotate(t.c);
}

void yaw(triangle& t, const coordinate& pivot, double theta){

    double c = cos(theta);
    double s = sin(theta);

    auto rotate = [&](coordinate& p) {
        double x = p.x - pivot.x;
        double y = p.y - pivot.y;

        double xr = x * c - y * s;
        double yr = x * s + y * c;

        p.x = xr + pivot.x;
        p.y = yr + pivot.y;
    };

    rotate(t.a);
    rotate(t.b);
    rotate(t.c);
}

void pitch(world& w, const coordinate& pivot, double theta){
    for (triangle& t : w.triangles){
        pitch(t, pivot, theta);
    }
}

void roll(world& w, const coordinate& pivot, double theta){
    for (triangle& t : w.triangles){
        roll(t, pivot, theta);
    }
}

void yaw(world& w, const coordinate& pivot, double theta){
    for (triangle& t : w.triangles){
        yaw(t, pivot, theta);
    }
}
