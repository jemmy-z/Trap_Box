//Jemmy Zhou
//cs-102, Fall 2016, period 4
//Trap_Box - Final Project
//Ball class

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/glut.H>
#include <FL/glu.h>
#include <FL/gl.h>
#include <cmath>
#include <stdlib.h>

#include "Ball.h"
#include "Object3D.h"

float xRotated = 0.0;
float yRotated = 0.0;
float zRotated = 0.0;
float radius = 0.2;

Ball::Ball(position p, color c, ball_direction d, float r) : Object3D(p, c)
{
    radius = r;
    dir = d;
}

ball_direction Ball::get_direction()
{
    return dir;
}

void Ball::set_direction(ball_direction d)
{
    dir = d;
}

float Ball::get_radius()
{
    return radius;
}

void Ball::draw()
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glColor3f(cor.r, cor.g, cor.b);
    glutSolidSphere(radius, 200, 200);
    glPopMatrix();
}
