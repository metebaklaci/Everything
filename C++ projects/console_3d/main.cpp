#include "renderer.h"
#include <windows.h>

int main(){

    world w;
    w.triangles.push_back(createTri(1,1,1, 1,-1,-1, -1,1,-1));
    w.triangles.push_back(createTri(1,1,1, 1,-1,-1, -1,-1,1));
    w.triangles.push_back(createTri(-1,1,-1, 1,-1,-1, -1,-1,1));
    w.triangles.push_back(createTri(-1,1,-1, 1,1,1, -1,-1,1));
    double d_theta = 0.2;
    coordinate pivot = createCoord(0,0,0);

    for(int i = 0; i < 100; i++){

        yaw(w, pivot, d_theta);
        roll(w, pivot, d_theta);
        render(w, 5, 10, 2, 2, 0.04, 0.1);
        Sleep(200);

    }

    return 0;
}