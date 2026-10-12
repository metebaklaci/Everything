#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

//struct definitions

struct coordinate{ //coordinate in 3D with x, y and z components. also serves as a vector.
    double x = 0;
    double y = 0;
    double z = 0;
};

struct triangle{ //triangle with vertices a, b and c
    coordinate a;
    coordinate b;
    coordinate c;
};

struct light{ //source of light (not yet functional)
    coordinate position;
    double intensity;
};

struct world{ //contains a set of triangles (and non-functional lights)
    std::vector<triangle> triangles = {};
    std::vector<light> lights = {};
};

struct line{ //line segment with endpoints q1 and q2
    coordinate q1;
    coordinate q2;
};

//functionality

coordinate createCoord(double x_in, double y_in, double z_in); //return a coordinate (x_in, y_in, z_in)
triangle createTri(double a, double b, double c, double d, double e, double f, double g, double h, double i); //return a triangle with vertices (a,b,c) (d,e,f) (g,h,i)

coordinate add_c(const coordinate& a, const coordinate& b); //vector addition a+b (coordinates taken as vectors)
coordinate sub_c(const coordinate& a, const coordinate& b); //vector subtraction a-b (coordinates taken as vectors)

double dot(const coordinate& a, const coordinate& b); //dot product of two vectors (coordinates taken as vectors)
coordinate cross(const coordinate& a, const coordinate& b); //cross product of two vectors (coordinates taken as vectors)

double signedVolume(const coordinate& a, const coordinate& b, const coordinate& c, const coordinate& d); //signed volume of tetrahedron whose vertices are a, b, c and d

bool intersect(const triangle& t, const line& l); //check if line l passes through triangle t

void roll(triangle& t, const coordinate& pivot, double theta); //roll t around pivot by angle theta (in rad)
void pitch(triangle& t, const coordinate& pivot, double theta); //pitch t around pivot by angle theta (in rad)
void yaw(triangle& t, const coordinate& pivot, double theta); //yaw t around pivot by angle theta (in rad)

void roll(world& w, const coordinate& pivot, double theta); //roll w around pivot by angle theta (in rad)
void pitch(world& w, const coordinate& pivot, double theta); //pitch w around pivot by angle theta (in rad)
void yaw(world& w, const coordinate& pivot, double theta); //yaw w around pivot by angle theta (in rad)

void render(const world& w, double camera_z, double visibility, double x, double y, double dx, double dy); //renders a given world with its triangles (and soon lights) in the terminal. an x:y ratio of 0.4 seems to work best for VSCode terminal.

#endif