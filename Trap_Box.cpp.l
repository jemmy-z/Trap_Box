1	//Jemmy Zhou
2	//cs-102, Fall 2016, period 4
3	//Trap_Box - Final Project
4	//Main file
5	
6	
7	//to compile, use msys:
8	//gcc `fltk-config --cxxflags` -c Trap_Box.cpp Box.cpp Ball.cpp 	Object3D.cpp
9	//g++ -o Trap_Box.exe Ball.o Box.o Trap_Box.o Object3D.o -mwindows `fltk-	config --ldflags --use-gl`
10	
    11	//to run:
    12	//Trap_Box.exe Trap_Box.cfg
    13	
    14	
    15	#include <FL/Fl.H>
    16	#include <FL/Fl_Window.H>
    17	#include <FL/Fl_Gl_Window.H>
    18	#include <FL/glut.H>
    19	#include <FL/glu.h>
    20	#include <FL/gl.h>
    21	#include <FL/Fl_Box.H>
    22	#include <FL/Fl_Button.H>
    23	#include <FL/fl_ask.H>
    24	
    25	#include <cstdlib>
    26	#include <stdlib.h>
    27	#include <stdio.h>
    28	#include <string.h>
    29	#include <config.h>
    30	#include <time.h>
    31	#include <math.h>
    32	#include <cmath>
    33	#include <vector>
    34	#include <iostream>
    35	#include <sstream>
    36	#include <fstream>
    37	#include <cstring>
    38	#include <iomanip>
    39	
    40	#include "Ball.h"
    41	#include "Box.h"
    42	#include "Object3D.h"
    43	#include "trackball.h"
    44	
    45	using namespace std;
    46	
    47	void exit_cb(Fl_Widget *, void *);
    48	void collision3D(float R, float m1, float m2, float r1, float r2, float& x1, float& y1,float& z1,
    49	                     float& x2, float& y2, float& z2, float& vx1, float& vy1, float& vz1,
    50	                     float& vx2, float& vy2, float& vz2, float d, int& error);
    51	void box_collision(Ball* sphere);
    52	void ball_collision(vector<Ball*> spheres, int i);
    53	void collision(vector<Ball*> spheres, int i);
    54	void redraw();
    55	void animate(void);
    56	void changeState(void);
    57	void motion(int x, int y);
    58	void mouse(int b, int s, int x, int y);
    59	void init(void);
    60	void visibility(int v);
    61	Ball* create_ball();
    62	void create_box();
    63	int string_to_int(string arg);
    64	float string_to_float(string arg);
    65	int locator(string line);
    66	void read_config(char** argv);
    67	
    68	int curX, curY, visible;
    69	static float curquat[4];
    70	static int doubleBuffer = 1;
    71	static int depth = 1;
    72	static int left_mouse, middle_mouse, mousex, mousey;
    73	static int spinning, sel_piece;
    74	static float lastquat[4];
    75	static GLint viewport[4];
    76	vector<Ball*> balls;
    77	Box *box;
    78	
    79	
    80	//gives a default setting in case no config file is found
    81	static int WINDOW_X = 1400; //window Width
    82	static int WINDOW_Y = 900; //window Height
    83	static int MAX_BALLS = 8; //maximum number of balls allowed (recommended < 10)
    84	static float BALL_SIZE = 0.3; //ball size (recommended 0.1 - 0.5)
    85	
    86	//instructions for the add ball button
    87	void add_ball_cb(Fl_Widget *, void *)
    88	{
    89	    if(balls.size() < MAX_BALLS) //checks whether max balls has been reached
    90	        balls.push_back(create_ball()); //creates a new ball
    91	    else //error message
    92	        fl_message("Maximum number of balls reached.");
    93	}
    94	
    95	//instructions for the exit button
    96	void exit_cb(Fl_Widget *, void *)
    97	{
    98	    exit(0); //exits window
    99	}
   100	
   101	/*function to calculate collisions in 3D copied from http://www.plasmaphysics.org.uk/programs/coll2d_cpp.htm
   102	Credits to Thomas Smid
   103	type double was changed to type float to improve performance
   104	the equation for calculating d is commented out for later edited use*/
   105	void collision3D(float R, float m1, float m2, float r1, float r2, float& x1, float& y1,float& z1,
   106	                     float& x2, float& y2, float& z2, float& vx1, float& vy1, float& vz1,
   107	                     float& vx2, float& vy2, float& vz2, float d, int& error)
   108	{
   109	    double  pi, r12, m21, v, theta2, phi2, st, ct, sp, cp, vx1r, vy1r, vz1r, fvz1r,
   110	            thetav, phiv, dr, alpha, beta, sbeta, cbeta, t, a, dvz2, vx2r, vy2r, vz2r,
   111	            x21, y21, z21, vx21, vy21, vz21, vx_cm, vy_cm, vz_cm;
   112	
   113	//  **** initialize some variables ****
   114	    pi=acos(-1.0E0);
   115	    error=0;
   116	    r12=r1+r2;
   117	    m21=m2/m1;
   118	    x21=x2-x1;
   119	    y21=y2-y1;
   120	    z21=z2-z1;
   121	    vx21=vx2-vx1;
   122	    vy21=vy2-vy1;
   123	    vz21=vz2-vz1;
   124	
   125	    vx_cm = (m1*vx1+m2*vx2)/(m1+m2) ;
   126	    vy_cm = (m1*vy1+m2*vy2)/(m1+m2) ;
   127	    vz_cm = (m1*vz1+m2*vz2)/(m1+m2) ;
   128	
   129	//  **** calculate relative speed ****
   130	//   d = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));
   131	
   132	    v=sqrt(vx21*vx21 +vy21*vy21 +vz21*vz21);
   133	
   134	/*  **** this block only if initial positions are given ****
   135	    if (mode != 'f') {
   136	       d=sqrt(x21*x21 +y21*y21);
   137	    }
   138	*/
   139	//  **** return if distance between balls smaller than sum of radii ****
   140	    if (d<r12)
   141	    {
   142	        error = 2;
   143	        return;
   144	    }
   145	
   146	//  **** return if relative speed = 0 ****
   147	    if (v==0)
   148	    {
   149	        error = 1;
   150	        return;
   151	    }
   152	
   153	//  **** shift coordinate system so that ball 1 is at the origin ****
   154	    x2=x21;
   155	    y2=y21;
   156	    z2=z21;
   157	
   158	//  **** boost coordinate system so that ball 2 is resting ****
   159	    vx1=-vx21;
   160	    vy1=-vy21;
   161	    vz1=-vz21;
   162	
   163	//  **** find the polar coordinates of the location of ball 2 ****
   164	    theta2=acos(z2/d);
   165	    if (x2==0 && y2==0) {phi2=0;} else {phi2=atan2(y2,x2);}
   166	    st=sin(theta2);
   167	    ct=cos(theta2);
   168	    sp=sin(phi2);
   169	    cp=cos(phi2);
   170	
   171	
   172	//  **** express the velocity vector of ball 1 in a rotated coordinate system where ball 2 lies on the z-axis ****
   173	    vx1r=ct*cp*vx1+ct*sp*vy1-st*vz1;
   174	    vy1r=cp*vy1-sp*vx1;
   175	    vz1r=st*cp*vx1+st*sp*vy1+ct*vz1;
   176	    fvz1r = vz1r/v ;
   177	    if (fvz1r>1) {fvz1r=1;}   // fix for possible rounding errors
   178	        else if (fvz1r<-1) {fvz1r=-1;}
   179	    thetav=acos(fvz1r);
   180	    if (vx1r==0 && vy1r==0) {phiv=0;}
   181	        else {phiv=atan2(vy1r,vx1r);}
   182	
   183	//  **** calculate the normalized impact parameter ****
   184	    dr=d*sin(thetav)/r12;
   185	
   186	
   187	//  **** return old positions and velocities if balls do not collide ****
   188	    if (thetav>pi/2 || fabs(dr)>1) {
   189	        x2=x2+x1;
   190	        y2=y2+y1;
   191	        z2=z2+z1;
   192	        vx1=vx1+vx2;
   193	        vy1=vy1+vy2;
   194	        vz1=vz1+vz2;
   195	        error=1;
   196	        return;
   197	    }
   198	
   199	//  **** calculate impact angles if balls do collide ****
   200	    alpha=asin(-dr);
   201	    beta=phiv;
   202	    sbeta=sin(beta);
   203	    cbeta=cos(beta);
   204	
   205	//  **** calculate time to collision ***
   206	    t=(d*cos(thetav) -r12*sqrt(1-dr*dr))/v;
   207	
   208	//  **** update positions and reverse the coordinate shift ****
   209	    x2=x2+vx2*t +x1;
   210	    y2=y2+vy2*t +y1;
   211	    z2=z2+vz2*t +z1;
   212	    x1=(vx1+vx2)*t +x1;
   213	    y1=(vy1+vy2)*t +y1;
   214	    z1=(vz1+vz2)*t +z1;
   215	
   216	//  ****  update velocities ****
   217	    a=tan(thetav+alpha);
   218	    dvz2=2*(vz1r+a*(cbeta*vx1r+sbeta*vy1r))/((1+a*a)*(1+m21));
   219	    vz2r=dvz2;
   220	    vx2r=a*cbeta*dvz2;
   221	    vy2r=a*sbeta*dvz2;
   222	    vz1r=vz1r-m21*vz2r;
   223	    vx1r=vx1r-m21*vx2r;
   224	    vy1r=vy1r-m21*vy2r;
   225	
   226	
   227	//  **** rotate the velocity vectors back and add the initial velocity vector of ball 2 to retrieve the original coordinate system ****
   228	    vx1=ct*cp*vx1r-sp*vy1r+st*cp*vz1r +vx2;
   229	    vy1=ct*sp*vx1r+cp*vy1r+st*sp*vz1r +vy2;
   230	    vz1=ct*vz1r-st*vx1r               +vz2;
   231	    vx2=ct*cp*vx2r-sp*vy2r+st*cp*vz2r +vx2;
   232	    vy2=ct*sp*vx2r+cp*vy2r+st*sp*vz2r +vy2;
   233	    vz2=ct*vz2r-st*vx2r               +vz2;
   234	
   235	//  ****  velocity correction for inelastic collisions ****
   236	    vx1=(vx1-vx_cm)*R + vx_cm;
   237	    vy1=(vy1-vy_cm)*R + vy_cm;
   238	    vz1=(vz1-vz_cm)*R + vz_cm;
   239	    vx2=(vx2-vx_cm)*R + vx_cm;
   240	    vy2=(vy2-vy_cm)*R + vy_cm;
   241	    vz2=(vz2-vz_cm)*R + vz_cm;
   242	
   243	    return; //error = 0
   244	}
   245	
   246	//checks and handles situations where the ball collides with virtual box
   247	void box_collision(Ball* sphere)
   248	{
   249	    struct position ball_p = sphere->get_position(); //gets position of ball
   250	    struct ball_direction ball_d = sphere->get_direction(); //gets direction of ball
   251	    float radius = sphere->get_radius(); //gets radius
   252	
   253	    float box_size = Box::BOX_SIZE; //gets box sizes
   254	    struct position box_position = box->get_position(); //gets box position
   255	
   256	    //tests if balls collide against yz-planes
   257	    if((ball_p.x + radius > box_position.x + box_size && ball_d.dx > 0)||
   258	       (ball_p.x - radius < box_position.x && ball_d.dx < 0))
   259	    {
   260	        ball_d.dx *= -1; //multiples x-direction by -1
   261	        sphere->set_direction(ball_d); //resets the direction in the object
   262	    }
   263	    else
   264	    {
   265	        ball_p.x += ball_d.dx; //keeps moving ball
   266	    }
   267	
   268	    //test if balls collide with xz-planes
   269	    if((-ball_p.y + radius > -box_position.y + box_size && ball_d.dy > 0)||
   270	       (-ball_p.y - radius < -box_position.y && ball_d.dy < 0))
   271	    {
   272	        ball_d.dy *= -1; //flips y direction
   273	        sphere->set_direction(ball_d); //resets the direction in the object
   274	    }
   275	    else
   276	    {
   277	        ball_p.y += -ball_d.dy; //keeps moving ball
   278	    }
   279	
   280	    //test if balls collide with xy-planes
   281	    if((ball_p.z + radius > box_position.z + box_size && ball_d.dz > 0)||
   282	       (ball_p.z - radius < box_position.z && ball_d.dz < 0))
   283	    {
   284	        ball_d.dz *= -1; //flips z direction
   285	        sphere->set_direction(ball_d); //resets the direction in the object
   286	    }
   287	    else
   288	    {
   289	        ball_p.z += ball_d.dz; //keeps moving ball
   290	    }
   291	
   292	    sphere->set_position(ball_p); //resets position into the object
   293	}
   294	
   295	//checks and handles situations where balls collide with balls
   296	void ball_collision(vector<Ball*> spheres, int i)
   297	{
   298	    struct position p_i = spheres[i]->get_position(); //gets position of ball 1
   299	    struct ball_direction d_i = spheres[i]->get_direction(); //gets direction of ball 1
   300	    float r_i = spheres[i]->get_radius(); //gets radius of ball 1
   301	
   302	    struct position p_j; //defines variable for position of ball 2
   303	    struct ball_direction d_j; //defines variable for direction of ball 2
   304	    float r_j; //defines variable for radius of ball 2
   305	
   306	    int error; //creates variable for later use
   307	
   308	    //runs a loops to check for collision with each ball that is in the box
   309	    for(unsigned int j = 0; j < spheres.size(); j++)
   310	    {
   311	        if(j == i) //cannot collide with itself
   312	        {
   313	            continue;
   314	        }
   315	        p_j = spheres[j]->get_position(); //gets position of ball 2
   316	        d_j = spheres[j]->get_direction(); //gets direction of ball 2
   317	        r_j = spheres[j]->get_radius(); //gets radius of ball 2
   318	
   319	        //calculates distance between the centers of both balls
   320	        float d = sqrt((p_j.x - p_i.x)*(p_j.x - p_i.x) +
   321	                       (p_j.y - p_i.y)*(p_j.y - p_i.y) +
   322	                       (p_j.z - p_i.z)*(p_j.z - p_i.z));
   323	
   324	        //if there's distance between balls, then won't collide
   325	        if(d > r_i + r_j)
   326	        {
   327	            continue;
   328	        }
   329	        else
   330	        {
   331	            //creates a buffer for a little error so the bouncing looks smoother
   332	            d = round(d * 10000) / 10000;
   333	            r_i = r_j = d/2;
   334	            p_i.y *= -1;
   335	            p_j.y *= -1;
   336	
   337	            //updates the value of error
   338	            //0 = collide, 2 = overlap, 1 = no collision
   339	            collision3D(1.0, 1.0, 1.0, r_i, r_j, p_i.x, p_i.y, p_i.z, p_j.x, p_j.y,
   340	                    p_j.z, d_i.dx, d_i.dy, d_i.dz, d_j.dx, d_j.dy, d_j.dz, d, error);
   341	
   342	            if(error == 0 || error == 2)
   343	            {
   344	                spheres[i]->set_direction(d_i);
   345	                spheres[j]->set_direction(d_j);
   346	            }
   347	        }
   348	    }
   349	}
   350	
   351	//checks for collision
   352	void collision(vector<Ball*> spheres, int i)
   353	{
   354	    box_collision(spheres[i]); //checks for ball to box collision
   355	    ball_collision(spheres, i); //checks for ball to ball collision
   356	}
   357	
   358	//draws everything in a frame
   359	void redraw()
   360	{
   361	    //sets camera
   362	    glMatrixMode(GL_PROJECTION);
   363	    glLoadIdentity();
   364	    gluPerspective(45, viewport[2] * 1.0/viewport[3], 0.1, 100.0);
   365	
   366	    float m[4][4];
   367	
   368	    //moves camera to the right position
   369	    build_rotmatrix(m, curquat);
   370	    glMatrixMode(GL_MODELVIEW);
   371	    glLoadIdentity();
   372	    glTranslatef(0, 0, -10);
   373	    glMultMatrixf(&(m[0][0]));
   374	    glRotatef(180, 0, 0, 1);
   375	
   376	    //clears last frame
   377	    if (depth)
   378	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   379	    else
   380	        glClear(GL_COLOR_BUFFER_BIT);
   381	
   382	    glLoadName(0);
   383	
   384	    box->draw(); //draws the box
   385	
   386	    //goes through all the balls that exist in the vector and draws it
   387	    for(unsigned int i = 0; i < balls.size(); i++)
   388	    {
   389	        Ball* sphere = balls[i]; //defines a temp ball
   390	        sphere->draw(); //draws the ball
   391	        collision(balls, i); //checks for collision
   392	    }
   393	    glFlush();
   394	
   395	    if (doubleBuffer)
   396	        glutSwapBuffers();
   397	    else
   398	        glFinish();
   399	}
   400	
   401	//animates the movement
   402	void animate(void)
   403	{
   404	    if (spinning)
   405	        add_quats(lastquat, curquat, curquat);
   406	    glutPostRedisplay();
   407	}
   408	
   409	//changes the state of the objects, either moving or still
   410	void changeState(void)
   411	{
   412	    if (visible)
   413	    {
   414	        if (!spinning)
   415	            glutIdleFunc(NULL);
   416	        else
   417	            glutIdleFunc(animate);
   418	    }
   419	    else
   420	        glutIdleFunc(NULL);
   421	}
   422	
   423	//calculates and implements the motion of the balls/box
   424	void motion(int x, int y)
   425	{
   426	    int W = WINDOW_X;
   427	    int H = WINDOW_Y;
   428	    if (middle_mouse && !left_mouse)
   429	    {
   430	        if (mousex != x || mousey != y)
   431	        {
   432	            trackball(lastquat, (2.0*mousex - W) / W, (H - 2.0*mousey) / H, (2.0*x - W) / W, (H - 2.0*y) / H);
   433	            spinning = 1;
   434	        }
   435	        else
   436	        {
   437	            spinning = 0;
   438	        }
   439	        changeState();
   440	    }
   441	    mousex = x;
   442	    mousey = y;
   443	    glutPostRedisplay();
   444	}
   445	
   446	//takes input of mouse to decide direction box is rotated
   447	void mouse(int b, int s, int x, int y)
   448	{
   449	    mousex = x;
   450	    mousey = y;
   451	    curX = x;
   452	    curY = y;
   453	
   454	    if (s == GLUT_DOWN)
   455	    {
   456	        switch (b)
   457	        {
   458	            case GLUT_LEFT_BUTTON:
   459	            left_mouse = GL_TRUE;
   460	            if (!sel_piece)
   461	            {
   462	                left_mouse = GL_FALSE;
   463	                middle_mouse = GL_TRUE; // let it rotate object
   464	            }
   465	            glutPostRedisplay();
   466	            break;
   467	
   468	            case GLUT_MIDDLE_BUTTON:
   469	            middle_mouse = GL_TRUE;
   470	            glutPostRedisplay();
   471	            break;
   472	        }
   473	    }
   474	    else
   475	    {
   476	        if (left_mouse)
   477	        {
   478	            left_mouse = GL_FALSE;
   479	            glutPostRedisplay();
   480	        }
   481	        else if (middle_mouse)
   482	        {
   483	            middle_mouse = GL_FALSE;
   484	            glutPostRedisplay();
   485	        }
   486	    }
   487	    motion(x, y);
   488	}
   489	
   490	//initializes everything needed in window
   491	void init(void)
   492	{
   493	    //initializes the light source
   494	    static float lmodel_ambient[] = {0.0, 0.0, 0.0, 0.0};
   495	    static float lmodel_twoside[] = {GL_FALSE};
   496	    static float lmodel_local[] = {GL_FALSE};
   497	    static float light0_ambient[] = {0.1, 0.1, 0.1, 1.0};
   498	    static float light0_diffuse[] = {1.0, 1.0, 1.0, 0.0};
   499	    static float light0_position[] = {0.8660254, 0.5, 1, 0};
   500	    static float light0_specular[] = {0.0, 0.0, 0.0, 0.0};
   501	    static float bevel_mat_ambient[] = {0.0, 0.0, 0.0, 1.0};
   502	    static float bevel_mat_shininess[] = {40.0};
   503	    static float bevel_mat_specular[] = {0.0, 0.0, 0.0, 0.0};
   504	    static float bevel_mat_diffuse[] = {1.0, 0.0, 0.0, 0.0};
   505	
   506	    glEnable(GL_CULL_FACE);
   507	    glCullFace(GL_BACK);
   508	    glEnable(GL_DEPTH_TEST);
   509	    glClearDepth(1.0);
   510	
   511	
   512	    glClearColor((float) rand()/RAND_MAX, (float) rand()/RAND_MAX, (float) rand()/RAND_MAX, 0.0); //window color
   513	    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
   514	    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   515	    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
   516	    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   517	    glEnable(GL_LIGHT0);
   518	
   519	    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lmodel_local);
   520	    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
   521	    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   522	    glEnable(GL_LIGHTING);
   523	
   524	    glMaterialfv(GL_FRONT, GL_AMBIENT, bevel_mat_ambient);
   525	    glMaterialfv(GL_FRONT, GL_SHININESS, bevel_mat_shininess);
   526	    glMaterialfv(GL_FRONT, GL_SPECULAR, bevel_mat_specular);
   527	    glMaterialfv(GL_FRONT, GL_DIFFUSE, bevel_mat_diffuse);
   528	
   529	    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
   530	    glEnable(GL_COLOR_MATERIAL);
   531	    glShadeModel(GL_FLAT);
   532	
   533	    //objects rotate around trackball
   534	    trackball(curquat, 0.0, 0.0, 0.0, 0.0);
   535	}
   536	
   537	//sets the visibility of everything in window
   538	void visibility(int v)
   539	{
   540	    if (v == GLUT_VISIBLE)
   541	        visible = 1;
   542	    else
   543	        visible = 0;
   544	    changeState();
   545	}
   546	
   547	//creates a ball
   548	Ball* create_ball()
   549	{
   550	    //sets starting position
   551	    struct position p;
   552	    p.x = (float) rand()/RAND_MAX * (Box::BOX_SIZE);
   553	    p.y = (float) rand()/RAND_MAX * (Box::BOX_SIZE);
   554	    p.z = (float) rand()/RAND_MAX * (Box::BOX_SIZE);
   555	
   556	    //sets color
   557	    struct color c;
   558	    c.r = (float) rand()/RAND_MAX;
   559	    c.g = (float) rand()/RAND_MAX;
   560	    c.b = (float) rand()/RAND_MAX;
   561	
   562	    //speed and direction ball will move in
   563	    struct ball_direction d;
   564	    d.dx = 0.01;
   565	    d.dy = 0.01;
   566	    d.dz = 0.01;
   567	
   568	    //size of ball
   569	    float r = BALL_SIZE;
   570	
   571	    //creates ball
   572	    Ball* sphere = new Ball(p, c, d, r);
   573	    return sphere;
   574	}
   575	
   576	//creates the box
   577	void create_box()
   578	{
   579	    //position of box
   580	    struct position p;
   581	    p.x = -2.0;
   582	    p.y = 2.5;
   583	    p.z = -0.5;
   584	
   585	    //color of box
   586	    struct color c;
   587	    c.r = (float) rand()/RAND_MAX;
   588	    c.g = (float) rand()/RAND_MAX;
   589	    c.b = (float) rand()/RAND_MAX;
   590	
   591	    //new box
   592	    box = new Box(p, c);
   593	}
   594	
   595	//finding numbers in config file
   596	int locator(string line)
   597	{
   598		int i = 0;
   599		while(line[i] != '=')
   600		{
   601			i++;
   602		}
   603	    while(!isdigit(line[i]))
   604		{
   605			i++;
   606		}
   607		return i;
   608	}
   609	
   610	//converts string from config file to int
   611	int string_to_int(string arg)
   612	{
   613	    istringstream strm;
   614	    strm.str(arg);
   615	    int temp = 0;
   616	    strm >> temp;
   617	    return temp;
   618	}
   619	
   620	//converts string from config file to float
   621	float string_to_float(string arg)
   622	{
   623	    istringstream strm;
   624	    strm.str(arg);
   625	    float temp = 0;
   626	    strm >> temp;
   627	    return temp;
   628	}
   629	
   630	//reads config file
   631	void read_config(char** argv)
   632	{
   633	    ifstream in_file;
   634	    string line;
   635	    in_file.open(argv[1]);
   636	    getline(in_file, line);
   637	    WINDOW_X = string_to_int(line.substr(locator(line), line.length() - 1));
   638	    getline(in_file, line);
   639	    WINDOW_Y = string_to_int(line.substr(locator(line), line.length() - 1));
   640	    getline(in_file, line);
   641	    MAX_BALLS = string_to_int(line.substr(locator(line), line.length() - 1));
   642	    getline(in_file, line);
   643	    BALL_SIZE = string_to_float(line.substr(locator(line), line.length() - 1));
   644	}
   645	
   646	int main(int argc, char** argv)
   647	{
   648	    //reads in file
   649	    ifstream in_file;
   650	    string line;
   651	    in_file.open(argv[1]);
   652		if(!in_file.fail()) //if no file, use default values
   653	        read_config(argv);
   654	
   655	    //defines random
   656	    srand((unsigned)time(NULL));
   657	
   658	    //display processor for low-performance computers
   659	    if (doubleBuffer)
   660	    {
   661	        glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
   662	    } else
   663	    {
   664	        glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_SINGLE | GLUT_MULTISAMPLE);
   665	    }
   666	
   667	    //button properties
   668	    double button_width = WINDOW_X / 10;
   669	    double button_height = WINDOW_Y / 20;
   670	
   671	    //creates FL Window
   672	    Fl_Window *window = new Fl_Window(50, 50, WINDOW_X, WINDOW_Y, "Boxes");
   673	    //creates 2 buttons
   674	    Fl_Button *b1 = new Fl_Button(WINDOW_X / 4 - button_width/2, WINDOW_X / 80, button_width, button_height, "Add Ball");
   675	    b1->callback(add_ball_cb,0);
   676	    Fl_Button *b2 = new Fl_Button((3*WINDOW_X)/4 - button_width/2, WINDOW_Y / 80, button_width, button_height, "Exit");
   677	    b2->callback(exit_cb,0);
   678	    //displays window
   679	    window->show();
   680	    window->begin();
   681	    //creates a glut Window on top of the FL Window
   682	    glutInitWindowSize(WINDOW_X , WINDOW_Y - WINDOW_Y / 12);
   683	    glutInitWindowPosition(0, WINDOW_Y / 12);
   684	    glutCreateWindow("");
   685	    visible = 1; //makes it visible
   686	
   687	    create_box(); //creates box
   688	
   689	    init(); //initializes window
   690	
   691	    //mimics mouse motion to start the box moving
   692	    trackball(lastquat, (2.0*484 - WINDOW_X) / WINDOW_X, (WINDOW_Y - 2.0*443) / WINDOW_Y,
   693	              (2.0*523 - WINDOW_X) / WINDOW_X, (WINDOW_Y - 2.0*432) / WINDOW_Y);
   694	    spinning = 1;
   695	    changeState();
   696	
   697	
   698	    glGetIntegerv(GL_VIEWPORT, viewport); //sets view
   699	    glutDisplayFunc(redraw); //draws objects
   700	    glutMotionFunc(motion); //moves everything
   701	    glutMouseFunc(mouse); //detects and handles mouse event
   702	    glutVisibilityFunc(visibility); //sets visible
   703	    glutMainLoop(); //loops everything
   704	
   705	    //deletes memory
   706	    for(int i = 0; i < 5; i++)
   707	        delete balls[i];
   708	    delete box;
   709	
   710	    return 0;
   711	}
