//Jemmy Zhou
//cs-102, Fall 2016, period 4
//Trap_Box - Final Project
//Object3D header file

#ifndef OBJECT3D_H
#define OBJECT3D_H

struct position
{
    float x;
    float y;
    float z;
};

struct color
{
    float r;
    float g;
    float b;
};

class Object3D
{
public:
    Object3D(position p, color c);

    position get_position();
    void set_position(position p);

    color get_color();
    void set_color(color c);

    position pos;
    color cor;
};

#endif // OBJECT3D

