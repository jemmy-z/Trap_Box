//Jemmy Zhou
//cs-102, Fall 2016, period 4
//Trap_Box - Final Project
//Ball header file

#ifndef BALL_H
#define BALL_H

#include "Object3D.h"


struct ball_direction
{
    float dx;
    float dy;
    float dz;
};

class Ball: public Object3D
{
public:
    Ball(position p, color c, ball_direction d, float r);

    ball_direction get_direction();
    void set_direction(ball_direction d);

    float get_radius();

    void draw();

private:
    double radius;
    ball_direction dir;
};

#endif // BALL_H
