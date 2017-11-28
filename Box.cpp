//Jemmy Zhou
//cs-102, Fall 2016, period 4
//Trap_Box - Final Project
//Box class

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <FL/glut.H>
#include <FL/glu.h>

#include "trackball.c"
#include "Box.h"
#include "Object3D.h"

#define WIDTH 4
#define HEIGHT 5
#define NCONTFACES (sizeof(containerfaces)/sizeof(containerfaces[0]))

static float curquat[4];

Box::Box(position p, color c) : Object3D(p, c)
{
    bx = 0;
    by = 0;
    bz = 0;;
    cor = c;

    t = Box::THICK;
    w = Box::BOX_SIZE;

    float temp[40][3] =
    {
        //Bottom Back Left
        {bx, by, bz},                           //0     Middle Point
        {bx - t, by, bz},                       //1     X change
        {bx, by - t, bz},                       //2     Y change
        {bx, by, bz - t},                       //3     Z change
        {bx - t, by - t, bz - t},               //4     All change

        //Bottom Front Left
        {bx + w, by, bz},                       //5     Middle Point
        {bx + w + t, by, bz},                   //6     X change
        {bx + w, by - t, bz},                   //7     Y change
        {bx + w, by, bz - t},                   //8     Z change
        {bx + w + t, by - t, bz - t},           //9     All change

        //Bottom Back Right
        {bx, by + w, bz},                       //10    Middle Point
        {bx - t, by + w, bz},                   //11    X change
        {bx, by + w + t, bz},                   //12    Y change
        {bx, by + w, bz - t},                   //13    Z change
        {bx - t, by + w + t, bz - t},           //14    All change

        //Bottom Front Right
        {bx + w, by + w, bz},                   //15    Middle Point
        {bx + w + t, by + w, bz},               //16    X change
        {bx + w, by + w + t, bz},               //17    Y change
        {bx + w, by + w, bz  - t},              //18    Z change
        {bx + w + t, by + w + t, bz - t},       //19    All change

        //Top Back Left
        {bx, by, bz + w},                       //20    Middle Point
        {bx - t, by, bz + w},                   //21    X change
        {bx, by - t, bz + w},                   //22    Y change
        {bx, by, bz + w + t},                   //23    Z change
        {bx - t, by - t, bz + w + t},           //24    All change

        //Top Front Left
        {bx + w, by, bz + w},                   //25    Middle Point
        {bx + w + t, by, bz + w},               //26    X change
        {bx + w, by - t, bz + w},               //27    Y change
        {bx + w, by, bz + w + t},               //28    Z change
        {bx + w + t, by - t, bz + w + t},       //29    All change

        //Top Back Right
        {bx, by + w, bz + w},                   //30    Middle Point
        {bx - t, by + w, bz + w},               //31    X change
        {bx, by + w + t, bz + w},               //32    Y change
        {bx, by + w, bz + w + t},               //33    Z change
        {bx - t, by + w + t, bz + w + t},       //34    All change

        //Top Front Right
        {bx + w, by + w, bz + w},               //35    Middle Point
        {bx + w + t, by + w, bz + w},           //36    X change
        {bx + w, by + w + t, bz + w},           //37    Y change
        {bx + w, by + w, bz + w + t},           //38    Z change
        {bx + w + t, by + w + t, bz + w + t},   //39    All change
    };

    memcpy(containercoords, temp, 120 * sizeof(float));
}


void Box::draw()
{
    glPushMatrix();

    float m[4][4];
    build_rotmatrix(m, curquat);
    glTranslatef(pos.x, pos.y, pos.z);
    glMultMatrixf(&(m[0][0]));
    glRotatef(0, 0, 0, 1);
    glLoadName(0);
    glBegin(GL_QUADS);
    glColor4f(cor.r, cor.g, cor.b, 0);
    for (int i = 0; i < signed NCONTFACES; i++)
    {
        glNormal3f(containernormals[i][0], -containernormals[i][1], containernormals[i][2]);
        for (int k = 3; k >= 0; k--)
        {
            glVertex3f(containercoords[containerfaces[i][k]][0], -(containercoords[containerfaces[i][k]][1]),
                       containercoords[containerfaces[i][k]][2]);
        }
    }
    glEnd();

    glPopMatrix();
}

