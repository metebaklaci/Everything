#include <cmath>

#ifndef FUNDAMENTALS_H

#define FUNDAMENTALS_H

struct vector3{
    double a;
    double b;
    double c;

    vector3(double a_, double b_, double c_){
        a = a_;
        b = b_;
        c = c_;
    }
};

struct matrix_3_3{
    vector3 a;
    vector3 b;
    vector3 c;
};

struct rigidbody{
    double i1;
    double i2;
    double i3;

    matrix_3_3 orientation;

    double w1;
    double w2;
    double w3;

    vector3 position;

    vector3 velocity;

    double mass;
};

vector3 add(const vector3& v1, const vector3& v2){
    return vector3(v1.a + v2.a, v1.b + v2.b, v1.c + v2.c);
}

vector3 scale(const vector3& v, const double s){
    return vector3(v.a * s, v.b * s, v.c * s);
}

vector3 subtract(const vector3& v1, const vector3& v2){
    return add(v1, scale(v2, -1));
}

double abs(const vector3& v){
    return sqrt(v.a * v.a + v.b * v.b + v.c * v.c);
}

#endif