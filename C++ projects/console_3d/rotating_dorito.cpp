#include "renderer.h"
#include <windows.h>

//Before you rotate the dorito, you need to build the dorito.
//Open a powershell terminal and run 

// .\build_the_dorito.ps1

//Then run

// .\rotating_dorito

int main(){

    world w;
    w.triangles.push_back(createTri(0,1,0, -0.8660254037844386,-0.5,0, 0.8660254037844386,-0.5,0));
    double d_theta = 0.2;
    coordinate pivot = createCoord(0,0,0);

    for(int i = 0; i < 100; i++){

        yaw(w, pivot, d_theta);
        roll(w, pivot, d_theta);
        render(w, 5, 10, 1.2, 1.2, 0.0333, 0.1);
        Sleep(200);

    }

    return 0;
}


/*

        #######
         #####################
          ##################################
           ###############################################
            ###########################################
             ########################################
              ####################################
               ################################
                ############################
                 ########################
                  #####################
                   #################
                    #############
                     #########
                      #####
                       ##

*/