//Jemmy Zhou
//cs-102, Fall 2016, period 4
//Trap_Box - Final Project
//Box header file

#ifndef BOX_H
#define BOX_H

#include "Object3D.h"

class Box : public Object3D
{
public:
    Box(position p, color c);

    float get_box_size();

    void draw();

    static const float BOX_SIZE = 3.5;
    static const float THICK = 0.1;

private:
    float bx;
    float by;
    float bz;
    float t;
    float w;
    color cor;

    float containercoords[40][3];

    const float containernormals[48][3] =
    {
        { -1,  0,  0},
        { -1,  0,  0},
        { -1,  0,  0},
        { -1,  0,  0},

        {  1,  0,  0},
        {  1,  0,  0},
        {  1,  0,  0},
        {  1,  0,  0},

        {  0, -1,  0},
        {  0, -1,  0},
        {  0, -1,  0},
        {  0, -1,  0},

        {  0,  1,  0},
        {  0,  1,  0},
        {  0,  1,  0},
        {  0,  1,  0},

        {  0,  0, -1},
        {  0,  0, -1},
        {  0,  0, -1},
        {  0,  0, -1},

        {  0,  0,  1},
        {  0,  0,  1},
        {  0,  0,  1},
        {  0,  0,  1},

        {  0,  0, -1},
        {  0,  0, -1},
        {  0,  0, -1},
        {  0,  0, -1},

        {  0,  0,  1},
        {  0,  0,  1},
        {  0,  0,  1},
        {  0,  0,  1},

        {  0,  1,  0},
        {  0,  1,  0},
        {  0,  1,  0},
        {  0,  1,  0},

        {  0, -1,  0},
        {  0, -1,  0},
        {  0, -1,  0},
        {  0, -1,  0},

        {  1,  0,  0},
        {  1,  0,  0},
        {  1,  0,  0},
        {  1,  0,  0},

        { -1,  0,  0},
        { -1,  0,  0},
        { -1,  0,  0},
        { -1,  0,  0},
    };

    const int containerfaces[48][4] =
    {
    //Back Plane
    {1, 11, 14, 4},         //Bottom
    {11, 31, 34, 14},       //Right
    {31, 21, 24, 34},       //Top
    {21, 1, 4, 24},         //Left

    //Front Plane
    {16, 6, 9, 19},         //Bottom
    {6, 26, 29, 9},         //Left
    {26, 36, 39, 29},       //Top
    {36, 16, 19, 39},       //Right

    //Left Plane
    {7, 2, 4, 9},           //Bottom
    {2, 22, 24, 4},         //Left
    {22, 27, 29, 24},       //Top
    {27, 7, 9, 29},         //Right

    //Right Plane
    {17, 37, 39, 19},       //Left
    {37, 32, 34, 39},       //Top
    {32, 12, 14, 34},       //Right
    {12, 17, 19, 14},       //Bottom

    //Bottom Plane
    {18, 13, 14, 19},       //Right
    {13, 3, 4, 14},         //Bottom
    {3, 8, 9, 4},           //Left
    {8, 18, 19, 9},         //Top

    //Top Plane
    {38, 28, 29, 39},       //Bottom
    {28, 23, 24, 29},       //Left
    {23, 33, 34, 24},       //Top
    {33, 38, 39, 34},       //Right

    //Down Rectangles
    {20, 21, 31, 30},       //Top
    {20, 25, 27, 22},       //Left
    {25, 35, 36, 26},       //Bottom
    {35, 30, 32, 37},       //Right

    //Up Rectangles
    {0, 10, 11, 1},         //Top
    {10, 15, 17, 12},       //Right
    {15, 5, 6, 16},         //Bottom
    {5, 0, 2, 7},         //Left

    //Left Rectangles
    {25, 20, 23, 28},       //Top
    {26, 6, 5, 25},         //Left
    {8, 3, 0, 5},           //Bottom
    {20, 0, 1, 21},         //Right

    //Right Rectangles
    {33, 30, 35, 38},       //Top
    {36, 35, 15, 16},       //Front
    {18, 15, 10, 13},       //Bottom
    {11, 10, 30, 31},       //Back

    //Front Rectangles
    {33, 23, 20, 30},       //Top
    {30, 10, 12, 32},       //Right
    {10, 0, 3, 13},         //Bottom
    {0, 20, 22, 2},         //Left

    //Back Rectangles
    {28, 38, 35, 25},       //Top
    {37, 17, 15, 35},       //Right
    {18, 8, 5, 15},         //Bottom
    {7, 27, 25, 5},         //Left
    };

};

#endif // BOX_H
