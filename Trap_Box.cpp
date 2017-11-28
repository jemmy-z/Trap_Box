//Jemmy Zhou
//cs-102, Fall 2016, period 4
//Trap_Box - Final Project
//Main file


//to compile, use msys:
//gcc `fltk-config --cxxflags` -c Trap_Box.cpp Box.cpp Ball.cpp Object3D.cpp
//g++ -o Trap_Box.exe Ball.o Box.o Trap_Box.o Object3D.o -mwindows `fltk-config --ldflags --use-gl`

//to run:
//Trap_Box.exe Trap_Box.cfg


#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/glut.H>
#include <FL/glu.h>
#include <FL/gl.h>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>

#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <config.h>
#include <time.h>
#include <math.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <iomanip>

#include "Ball.h"
#include "Box.h"
#include "Object3D.h"
#include "trackball.h"

using namespace std;

void exit_cb(Fl_Widget *, void *);
void collision3D(float R, float m1, float m2, float r1, float r2, float& x1, float& y1,float& z1,
                     float& x2, float& y2, float& z2, float& vx1, float& vy1, float& vz1,
                     float& vx2, float& vy2, float& vz2, float d, int& error);
void box_collision(Ball* sphere);
void ball_collision(vector<Ball*> spheres, int i);
void collision(vector<Ball*> spheres, int i);
void redraw();
void animate(void);
void changeState(void);
void motion(int x, int y);
void mouse(int b, int s, int x, int y);
void init(void);
void visibility(int v);
Ball* create_ball();
void create_box();
int string_to_int(string arg);
float string_to_float(string arg);
int locator(string line);
void read_config(char** argv);

int curX, curY, visible;
static float curquat[4];
static int doubleBuffer = 1;
static int depth = 1;
static int left_mouse, middle_mouse, mousex, mousey;
static int spinning, sel_piece;
static float lastquat[4];
static GLint viewport[4];
vector<Ball*> balls;
Box *box;


//gives a default setting in case no config file is found
static int WINDOW_X = 1400; //window Width
static int WINDOW_Y = 900; //window Height
static int MAX_BALLS = 8; //maximum number of balls allowed (recommended < 10)
static float BALL_SIZE = 0.3; //ball size (recommended 0.1 - 0.5)

//instructions for the add ball button
void add_ball_cb(Fl_Widget *, void *)
{
    if(balls.size() < MAX_BALLS) //checks whether max balls has been reached
        balls.push_back(create_ball()); //creates a new ball
    else //error message
        fl_message("Maximum number of balls reached.");
}

//instructions for the exit button
void exit_cb(Fl_Widget *, void *)
{
    exit(0); //exits window
}

/*function to calculate collisions in 3D copied from http://www.plasmaphysics.org.uk/programs/coll2d_cpp.htm
Credits to Thomas Smid
type double was changed to type float to improve performance
the equation for calculating d is commented out for later edited use*/
void collision3D(float R, float m1, float m2, float r1, float r2, float& x1, float& y1,float& z1,
                     float& x2, float& y2, float& z2, float& vx1, float& vy1, float& vz1,
                     float& vx2, float& vy2, float& vz2, float d, int& error)
{
    double  pi, r12, m21, v, theta2, phi2, st, ct, sp, cp, vx1r, vy1r, vz1r, fvz1r,
            thetav, phiv, dr, alpha, beta, sbeta, cbeta, t, a, dvz2, vx2r, vy2r, vz2r,
            x21, y21, z21, vx21, vy21, vz21, vx_cm, vy_cm, vz_cm;

//  **** initialize some variables ****
    pi=acos(-1.0E0);
    error=0;
    r12=r1+r2;
    m21=m2/m1;
    x21=x2-x1;
    y21=y2-y1;
    z21=z2-z1;
    vx21=vx2-vx1;
    vy21=vy2-vy1;
    vz21=vz2-vz1;

    vx_cm = (m1*vx1+m2*vx2)/(m1+m2) ;
    vy_cm = (m1*vy1+m2*vy2)/(m1+m2) ;
    vz_cm = (m1*vz1+m2*vz2)/(m1+m2) ;

//  **** calculate relative speed ****
//   d = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));

    v=sqrt(vx21*vx21 +vy21*vy21 +vz21*vz21);

/*  **** this block only if initial positions are given ****
    if (mode != 'f') {
       d=sqrt(x21*x21 +y21*y21);
    }
*/
//  **** return if distance between balls smaller than sum of radii ****
    if (d<r12)
    {
        error = 2;
        return;
    }

//  **** return if relative speed = 0 ****
    if (v==0)
    {
        error = 1;
        return;
    }

//  **** shift coordinate system so that ball 1 is at the origin ****
    x2=x21;
    y2=y21;
    z2=z21;

//  **** boost coordinate system so that ball 2 is resting ****
    vx1=-vx21;
    vy1=-vy21;
    vz1=-vz21;

//  **** find the polar coordinates of the location of ball 2 ****
    theta2=acos(z2/d);
    if (x2==0 && y2==0) {phi2=0;} else {phi2=atan2(y2,x2);}
    st=sin(theta2);
    ct=cos(theta2);
    sp=sin(phi2);
    cp=cos(phi2);


//  **** express the velocity vector of ball 1 in a rotated coordinate system where ball 2 lies on the z-axis ****
    vx1r=ct*cp*vx1+ct*sp*vy1-st*vz1;
    vy1r=cp*vy1-sp*vx1;
    vz1r=st*cp*vx1+st*sp*vy1+ct*vz1;
    fvz1r = vz1r/v ;
    if (fvz1r>1) {fvz1r=1;}   // fix for possible rounding errors
        else if (fvz1r<-1) {fvz1r=-1;}
    thetav=acos(fvz1r);
    if (vx1r==0 && vy1r==0) {phiv=0;}
        else {phiv=atan2(vy1r,vx1r);}

//  **** calculate the normalized impact parameter ****
    dr=d*sin(thetav)/r12;


//  **** return old positions and velocities if balls do not collide ****
    if (thetav>pi/2 || fabs(dr)>1) {
        x2=x2+x1;
        y2=y2+y1;
        z2=z2+z1;
        vx1=vx1+vx2;
        vy1=vy1+vy2;
        vz1=vz1+vz2;
        error=1;
        return;
    }

//  **** calculate impact angles if balls do collide ****
    alpha=asin(-dr);
    beta=phiv;
    sbeta=sin(beta);
    cbeta=cos(beta);

//  **** calculate time to collision ***
    t=(d*cos(thetav) -r12*sqrt(1-dr*dr))/v;

//  **** update positions and reverse the coordinate shift ****
    x2=x2+vx2*t +x1;
    y2=y2+vy2*t +y1;
    z2=z2+vz2*t +z1;
    x1=(vx1+vx2)*t +x1;
    y1=(vy1+vy2)*t +y1;
    z1=(vz1+vz2)*t +z1;

//  ****  update velocities ****
    a=tan(thetav+alpha);
    dvz2=2*(vz1r+a*(cbeta*vx1r+sbeta*vy1r))/((1+a*a)*(1+m21));
    vz2r=dvz2;
    vx2r=a*cbeta*dvz2;
    vy2r=a*sbeta*dvz2;
    vz1r=vz1r-m21*vz2r;
    vx1r=vx1r-m21*vx2r;
    vy1r=vy1r-m21*vy2r;


//  **** rotate the velocity vectors back and add the initial velocity vector of ball 2 to retrieve the original coordinate system ****
    vx1=ct*cp*vx1r-sp*vy1r+st*cp*vz1r +vx2;
    vy1=ct*sp*vx1r+cp*vy1r+st*sp*vz1r +vy2;
    vz1=ct*vz1r-st*vx1r               +vz2;
    vx2=ct*cp*vx2r-sp*vy2r+st*cp*vz2r +vx2;
    vy2=ct*sp*vx2r+cp*vy2r+st*sp*vz2r +vy2;
    vz2=ct*vz2r-st*vx2r               +vz2;

//  ****  velocity correction for inelastic collisions ****
    vx1=(vx1-vx_cm)*R + vx_cm;
    vy1=(vy1-vy_cm)*R + vy_cm;
    vz1=(vz1-vz_cm)*R + vz_cm;
    vx2=(vx2-vx_cm)*R + vx_cm;
    vy2=(vy2-vy_cm)*R + vy_cm;
    vz2=(vz2-vz_cm)*R + vz_cm;

    return; //error = 0
}

//checks and handles situations where the ball collides with virtual box
void box_collision(Ball* sphere)
{
    struct position ball_p = sphere->get_position(); //gets position of ball
    struct ball_direction ball_d = sphere->get_direction(); //gets direction of ball
    float radius = sphere->get_radius(); //gets radius

    float box_size = Box::BOX_SIZE; //gets box sizes
    struct position box_position = box->get_position(); //gets box position

    //tests if balls collide against yz-planes
    if((ball_p.x + radius > box_position.x + box_size && ball_d.dx > 0)||
       (ball_p.x - radius < box_position.x && ball_d.dx < 0))
    {
        ball_d.dx *= -1; //multiples x-direction by -1
        sphere->set_direction(ball_d); //resets the direction in the object
    }
    else
    {
        ball_p.x += ball_d.dx; //keeps moving ball
    }

    //test if balls collide with xz-planes
    if((-ball_p.y + radius > -box_position.y + box_size && ball_d.dy > 0)||
       (-ball_p.y - radius < -box_position.y && ball_d.dy < 0))
    {
        ball_d.dy *= -1; //flips y direction
        sphere->set_direction(ball_d); //resets the direction in the object
    }
    else
    {
        ball_p.y += -ball_d.dy; //keeps moving ball
    }

    //test if balls collide with xy-planes
    if((ball_p.z + radius > box_position.z + box_size && ball_d.dz > 0)||
       (ball_p.z - radius < box_position.z && ball_d.dz < 0))
    {
        ball_d.dz *= -1; //flips z direction
        sphere->set_direction(ball_d); //resets the direction in the object
    }
    else
    {
        ball_p.z += ball_d.dz; //keeps moving ball
    }

    sphere->set_position(ball_p); //resets position into the object
}

//checks and handles situations where balls collide with balls
void ball_collision(vector<Ball*> spheres, int i)
{
    struct position p_i = spheres[i]->get_position(); //gets position of ball 1
    struct ball_direction d_i = spheres[i]->get_direction(); //gets direction of ball 1
    float r_i = spheres[i]->get_radius(); //gets radius of ball 1

    struct position p_j; //defines variable for position of ball 2
    struct ball_direction d_j; //defines variable for direction of ball 2
    float r_j; //defines variable for radius of ball 2

    int error; //creates variable for later use

    //runs a loops to check for collision with each ball that is in the box
    for(unsigned int j = 0; j < spheres.size(); j++)
    {
        if(j == i) //cannot collide with itself
        {
            continue;
        }
        p_j = spheres[j]->get_position(); //gets position of ball 2
        d_j = spheres[j]->get_direction(); //gets direction of ball 2
        r_j = spheres[j]->get_radius(); //gets radius of ball 2

        //calculates distance between the centers of both balls
        float d = sqrt((p_j.x - p_i.x)*(p_j.x - p_i.x) +
                       (p_j.y - p_i.y)*(p_j.y - p_i.y) +
                       (p_j.z - p_i.z)*(p_j.z - p_i.z));

        //if there's distance between balls, then won't collide
        if(d > r_i + r_j)
        {
            continue;
        }
        else
        {
            //creates a buffer for a little error so the bouncing looks smoother
            d = round(d * 10000) / 10000;
            r_i = r_j = d/2;
            p_i.y *= -1;
            p_j.y *= -1;

            //updates the value of error
            //0 = collide, 2 = overlap, 1 = no collision
            collision3D(1.0, 1.0, 1.0, r_i, r_j, p_i.x, p_i.y, p_i.z, p_j.x, p_j.y,
                    p_j.z, d_i.dx, d_i.dy, d_i.dz, d_j.dx, d_j.dy, d_j.dz, d, error);

            if(error == 0 || error == 2)
            {
                spheres[i]->set_direction(d_i);
                spheres[j]->set_direction(d_j);
            }
        }
    }
}

//checks for collision
void collision(vector<Ball*> spheres, int i)
{
    box_collision(spheres[i]); //checks for ball to box collision
    ball_collision(spheres, i); //checks for ball to ball collision
}

//draws everything in a frame
void redraw()
{
    //sets camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, viewport[2] * 1.0/viewport[3], 0.1, 100.0);

    float m[4][4];

    //moves camera to the right position
    build_rotmatrix(m, curquat);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -10);
    glMultMatrixf(&(m[0][0]));
    glRotatef(180, 0, 0, 1);

    //clears last frame
    if (depth)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else
        glClear(GL_COLOR_BUFFER_BIT);

    glLoadName(0);

    box->draw(); //draws the box

    //goes through all the balls that exist in the vector and draws it
    for(unsigned int i = 0; i < balls.size(); i++)
    {
        Ball* sphere = balls[i]; //defines a temp ball
        sphere->draw(); //draws the ball
        collision(balls, i); //checks for collision
    }
    glFlush();

    if (doubleBuffer)
        glutSwapBuffers();
    else
        glFinish();
}

//animates the movement
void animate(void)
{
    if (spinning)
        add_quats(lastquat, curquat, curquat);
    glutPostRedisplay();
}

//changes the state of the objects, either moving or still
void changeState(void)
{
    if (visible)
    {
        if (!spinning)
            glutIdleFunc(NULL);
        else
            glutIdleFunc(animate);
    }
    else
        glutIdleFunc(NULL);
}

//calculates and implements the motion of the balls/box
void motion(int x, int y)
{
    int W = WINDOW_X;
    int H = WINDOW_Y;
    if (middle_mouse && !left_mouse)
    {
        if (mousex != x || mousey != y)
        {
            trackball(lastquat, (2.0*mousex - W) / W, (H - 2.0*mousey) / H, (2.0*x - W) / W, (H - 2.0*y) / H);
            spinning = 1;
        }
        else
        {
            spinning = 0;
        }
        changeState();
    }
    mousex = x;
    mousey = y;
    glutPostRedisplay();
}

//takes input of mouse to decide direction box is rotated
void mouse(int b, int s, int x, int y)
{
    mousex = x;
    mousey = y;
    curX = x;
    curY = y;

    if (s == GLUT_DOWN)
    {
        switch (b)
        {
            case GLUT_LEFT_BUTTON:
            left_mouse = GL_TRUE;
            if (!sel_piece)
            {
                left_mouse = GL_FALSE;
                middle_mouse = GL_TRUE; // let it rotate object
            }
            glutPostRedisplay();
            break;

            case GLUT_MIDDLE_BUTTON:
            middle_mouse = GL_TRUE;
            glutPostRedisplay();
            break;
        }
    }
    else
    {
        if (left_mouse)
        {
            left_mouse = GL_FALSE;
            glutPostRedisplay();
        }
        else if (middle_mouse)
        {
            middle_mouse = GL_FALSE;
            glutPostRedisplay();
        }
    }
    motion(x, y);
}

//initializes everything needed in window
void init(void)
{
    //initializes the light source
    static float lmodel_ambient[] = {0.0, 0.0, 0.0, 0.0};
    static float lmodel_twoside[] = {GL_FALSE};
    static float lmodel_local[] = {GL_FALSE};
    static float light0_ambient[] = {0.1, 0.1, 0.1, 1.0};
    static float light0_diffuse[] = {1.0, 1.0, 1.0, 0.0};
    static float light0_position[] = {0.8660254, 0.5, 1, 0};
    static float light0_specular[] = {0.0, 0.0, 0.0, 0.0};
    static float bevel_mat_ambient[] = {0.0, 0.0, 0.0, 1.0};
    static float bevel_mat_shininess[] = {40.0};
    static float bevel_mat_specular[] = {0.0, 0.0, 0.0, 0.0};
    static float bevel_mat_diffuse[] = {1.0, 0.0, 0.0, 0.0};

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);


    glClearColor((float) rand()/RAND_MAX, (float) rand()/RAND_MAX, (float) rand()/RAND_MAX, 0.0); //window color
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glEnable(GL_LIGHT0);

    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lmodel_local);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glEnable(GL_LIGHTING);

    glMaterialfv(GL_FRONT, GL_AMBIENT, bevel_mat_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, bevel_mat_shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, bevel_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bevel_mat_diffuse);

    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_FLAT);

    //objects rotate around trackball
    trackball(curquat, 0.0, 0.0, 0.0, 0.0);
}

//sets the visibility of everything in window
void visibility(int v)
{
    if (v == GLUT_VISIBLE)
        visible = 1;
    else
        visible = 0;
    changeState();
}

//creates a ball
Ball* create_ball()
{
    //sets starting position
    struct position p;
    p.x = (float) rand()/RAND_MAX * (Box::BOX_SIZE);
    p.y = (float) rand()/RAND_MAX * (Box::BOX_SIZE);
    p.z = (float) rand()/RAND_MAX * (Box::BOX_SIZE);

    //sets color
    struct color c;
    c.r = (float) rand()/RAND_MAX;
    c.g = (float) rand()/RAND_MAX;
    c.b = (float) rand()/RAND_MAX;

    //speed and direction ball will move in
    struct ball_direction d;
    d.dx = 0.01;
    d.dy = 0.01;
    d.dz = 0.01;

    //size of ball
    float r = BALL_SIZE;

    //creates ball
    Ball* sphere = new Ball(p, c, d, r);
    return sphere;
}

//creates the box
void create_box()
{
    //position of box
    struct position p;
    p.x = -2.0;
    p.y = 2.5;
    p.z = -0.5;

    //color of box
    struct color c;
    c.r = (float) rand()/RAND_MAX;
    c.g = (float) rand()/RAND_MAX;
    c.b = (float) rand()/RAND_MAX;

    //new box
    box = new Box(p, c);
}

//finding numbers in config file
int locator(string line)
{
	int i = 0;
	while(line[i] != '=')
	{
		i++;
	}
    while(!isdigit(line[i]))
	{
		i++;
	}
	return i;
}

//converts string from config file to int
int string_to_int(string arg)
{
    istringstream strm;
    strm.str(arg);
    int temp = 0;
    strm >> temp;
    return temp;
}

//converts string from config file to float
float string_to_float(string arg)
{
    istringstream strm;
    strm.str(arg);
    float temp = 0;
    strm >> temp;
    return temp;
}

//reads config file
void read_config(char** argv)
{
    ifstream in_file;
    string line;
    in_file.open(argv[1]);
    getline(in_file, line);
    WINDOW_X = string_to_int(line.substr(locator(line), line.length() - 1));
    getline(in_file, line);
    WINDOW_Y = string_to_int(line.substr(locator(line), line.length() - 1));
    getline(in_file, line);
    MAX_BALLS = string_to_int(line.substr(locator(line), line.length() - 1));
    getline(in_file, line);
    BALL_SIZE = string_to_float(line.substr(locator(line), line.length() - 1));
}

int main(int argc, char** argv)
{
    //reads in file
    ifstream in_file;
    string line;
    in_file.open(argv[1]);
	if(!in_file.fail()) //if no file, use default values
        read_config(argv);

    //defines random
    srand((unsigned)time(NULL));

    //display processor for low-performance computers
    if (doubleBuffer)
    {
        glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    } else
    {
        glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_SINGLE | GLUT_MULTISAMPLE);
    }

    //button properties
    double button_width = WINDOW_X / 10;
    double button_height = WINDOW_Y / 20;

    //creates FL Window
    Fl_Window *window = new Fl_Window(50, 50, WINDOW_X, WINDOW_Y, "Boxes");
    //creates 2 buttons
    Fl_Button *b1 = new Fl_Button(WINDOW_X / 4 - button_width/2, WINDOW_X / 80, button_width, button_height, "Add Ball");
    b1->callback(add_ball_cb,0);
    Fl_Button *b2 = new Fl_Button((3*WINDOW_X)/4 - button_width/2, WINDOW_Y / 80, button_width, button_height, "Exit");
    b2->callback(exit_cb,0);
    //displays window
    window->show();
    window->begin();
    //creates a glut Window on top of the FL Window
    glutInitWindowSize(WINDOW_X , WINDOW_Y - WINDOW_Y / 12);
    glutInitWindowPosition(0, WINDOW_Y / 12);
    glutCreateWindow("");
    visible = 1; //makes it visible

    create_box(); //creates box

    init(); //initializes window

    //mimics mouse motion to start the box moving
    trackball(lastquat, (2.0*484 - WINDOW_X) / WINDOW_X, (WINDOW_Y - 2.0*443) / WINDOW_Y,
              (2.0*523 - WINDOW_X) / WINDOW_X, (WINDOW_Y - 2.0*432) / WINDOW_Y);
    spinning = 1;
    changeState();


    glGetIntegerv(GL_VIEWPORT, viewport); //sets view
    glutDisplayFunc(redraw); //draws objects
    glutMotionFunc(motion); //moves everything
    glutMouseFunc(mouse); //detects and handles mouse event
    glutVisibilityFunc(visibility); //sets visible
    glutMainLoop(); //loops everything

    //deletes memory
    for(int i = 0; i < 5; i++)
        delete balls[i];
    delete box;

    return 0;
}
