//Jemmy Zhou
//cs-102, Fall 2016, period 4
//Trap_Box - Final Project
//Object3D class


#include "Object3D.h"
#include "Ball.h"

Object3D::Object3D(position p, color c)
{
    pos = p;
    cor = c;
}

position Object3D::get_position()
{
    return pos;
}

void Object3D::set_position(position p)
{
    pos = p;
}

color Object3D::get_color()
{
    return cor;
}

void Object3D::set_color(color c)
{
    cor = c;
}
