#include <vector>
#include "fundamentals.h"

#ifndef UNIVERSE_H

#define UNIVERSE_H

class Universe{

    public:
    std::vector<rigidbody> bodies = {};

    double timestep = 1/1048576;

    double time = 0;

    double G = 6.674e-11;

    void updatePositions_euler(){
        std::vector<vector3> accelerations;
        for(int i = 0; i < bodies.size(); ++i){
            vector3 force = vector3(0,0,0);
            for(int j = 0; j < bodies.size(); ++j){
                if(i != j){
                    vector3 gravity = scale(scale(subtract(bodies[j].position, bodies[i].position), pow(abs(subtract(bodies[j].position, bodies[i].position)), -3)), bodies[i].mass * bodies[j].mass * G);
                    //subtract() finds vector from bodies[i] to bodies[j].
                    //first scale() scales the vector down by the cube of their absolute value
                    //since subtract() will return vector with magnitude equal to distance
                    //and gravity is proportional to inverse square of distance.
                    //second scale() scales vector by physical constants (G*m1*m2)

                    force = add(force, gravity);
                }
            }

            //any additional forces may be added here

            accelerations.push_back(scale(force, 1/bodies[i].mass));
        }

        //to make euler's method most accurate, the displacement will be updated by the velocity found with half acceleration

        for(int i = 0; i < bodies.size(); ++i){
            bodies[i].velocity = add(bodies[i].velocity, scale(scale(accelerations[i], 0.5), timestep));
            bodies[i].position = add(bodies[i].position, scale(bodies[i].velocity, timestep));
            bodies[i].velocity = add(bodies[i].velocity, scale(scale(accelerations[i], 0.5), timestep));
        }

    }

    void updateOrientation_euler(){
        
    }

};

#endif