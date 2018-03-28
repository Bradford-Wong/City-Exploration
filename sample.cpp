#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
using namespace std;
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "sphere.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#define BLADE_RADIUS		 1.0
#define BLADE_WIDTH		 0.4

//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "Final Project -- Bradford Wong" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b

float helicopterX = 0, helicopterY = 20, helicopterZ = 0;
// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
int		viewMode = 0;
GLuint	BoxList;				// object display list
GLuint	HeliList;
GLuint	BladeList;
GLuint  TorusList;
GLuint	EarthList;
GLuint	StreetList;
GLuint	BuildingList;
GLuint	WallList;
GLuint	Grass;
GLuint	PersonList;
GLuint	FalconList;

int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
float	Time; //time variable
int		BladeAngle = 10;
bool	Light0On = true;
bool	Light2On = true;
bool	Light1On = true;
bool	LightTraffic = true;
bool	Freeze = false;
float	xCube=0, yCube=0, zCube=0;
float	xLight0 = xCube, yLight0 = yCube, zLight0 = zCube + .2; //red
float	xLight1=0, yLight1=2, zLight1=0; //white
float	xLight2=0, yLight2=0, zLight2=-4; // yellow
float	zMove = 0;
float ipersonX = 0;
float ipersonY = 0;
float ipersonZ = 0;

float	sunX = 0;
float	sunY = 70;
float sunZ = 0;

#include "heli550.h"

// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );
void	SetMaterial(float, float, float, float);
float *Array3(float a, float b, float c);
float *MulArray3(float factor, float array0[3]);
void SetPointLight(int, float, float, float, float, float, float);
void SetSpotLight(int, float, float, float, float, float, float, float, float, float);
// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

	InitLists( );


	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );


	// setup all the user interface stuff:

	InitMenus( );


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );


	// this is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// force a call to Display( ) next time it is convenient:
	int ms = glutGet(GLUT_ELAPSED_TIME);

	float Time2;
#define MS_IN_THE_ANIMATION_CYCLE	10000


	int ms2 = glutGet(GLUT_ELAPSED_TIME);	// milliseconds
	ms2 %= MS_IN_THE_ANIMATION_CYCLE;
	Time2 = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;        // [ 0., 1. )
	BladeAngle = 5000 * Time2;


	//ms %= MS_PER_CYCLE;
	ms /= 1000;
	ms %= 88;
	Time = ms;		// [0.,1.)
					//cout << Time << endl;
					//cout << Time << endl;
	//cout << Time << endl;
	if (Time < 22) {
		ipersonZ -= .03;
		helicopterZ -= .2;
		helicopterY -= .03;
		sunX += .2;
	
	}
	else if (Time < 44) {
		ipersonX += 0.03;
		helicopterX -= .2;
	}
	else if (Time < 66) {
		ipersonZ += .03;
		helicopterZ += .2;
		helicopterY += .03;
		sunX -= .2;
		
	}
	else if (Time < 88) {
		ipersonX -= .03;
		helicopterX += .2;
	}
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


float White[] = { 1.,1.,1.,1. };
// utility to create an array from 3 separate values:
float *
Array3(float a, float b, float c)
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}
// utility to create an array from a multiplier and an array:
float *
MulArray3(float factor, float array0[3])
{
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

void
SetMaterial(float r, float g, float b, float shininess)
{
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 2.f);
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void
SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}



void
SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
	glLightf(ilight, GL_SPOT_EXPONENT, 1.);
	glLightf(ilight, GL_SPOT_CUTOFF, 45.);
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

// draw the complete scene:


void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( DepthBufferOn != 0 )
		glEnable( GL_DEPTH_TEST );
	else
		glDisable( GL_DEPTH_TEST );


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );



	// set the eye position, look-at position, and up-vector:

	if (viewMode == 0) {
		gluLookAt(0., 0., 3., 0., 0., 0., 0., 1., 0.);
	}
	else {
		if (Time < 22)
			gluLookAt(ipersonX, .25, ipersonZ-2, ipersonX, .25, ipersonZ -6, 0., 1., 0.);
		else if(Time < 44)
			gluLookAt(ipersonX + 2, ipersonY + .25, ipersonZ, ipersonX + 6, ipersonY + .25, ipersonZ, 0., 1., 0.);
		else if (Time < 66)
			gluLookAt(ipersonX, ipersonY + .25, ipersonZ + 2, ipersonX, ipersonY + .25, ipersonZ + 6, 0., 1., 0.);
		else if (Time < 88)
			gluLookAt(ipersonX - 2, ipersonY + .25, ipersonZ, ipersonX - 6, ipersonY + .25, ipersonZ, 0., 1., 0.);
	}


	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );


	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );


	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	// possibly draw the axes:
/*
	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}*/


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable( GL_NORMALIZE );


	glPushMatrix();
	glTranslatef(helicopterX, helicopterY, helicopterZ);
	glCallList(HeliList);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	if (Light0On)
		glEnable(GL_LIGHT0);
	if (Light1On)
		glEnable(GL_LIGHT1);
	if (LightTraffic && Time < 22)
		glEnable(GL_LIGHT2);
	else if (Time < 66 && Time > 44 && LightTraffic)
		glEnable(GL_LIGHT3);
	else if (Time < 88 && Time > 66 && LightTraffic)
		glEnable(GL_LIGHT4);

	glEnable(GL_LIGHT5);
	glEnable(GL_LIGHT6);


	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MulArray3(.3f, White));
	

	glCallList(EarthList);
	glCallList(FalconList);
	glCallList(StreetList);
	glCallList(BuildingList);
	glCallList(WallList);

	
	
	//draw grass
	glPushMatrix();
	glTranslatef(2, -.25, 0);
	
	glPopMatrix();
	glCallList(Grass);

	
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);


	//glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);



	int light2x = 0, light2y = 0, light2z = 0;
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);

	//Create light from helicopter
	/*glPushMatrix();
	glTranslatef(0, 10, -40);
	glColor3f(0, 0, 1);
	glutSolidSphere(.25, 100, 100);
	glPopMatrix();
	glEnable(GL_LIGHT6);
	SetSpotLight(GL_LIGHT6, 0, 10, -40, 0, 0, -40, 0, 0, 1);*/

	//Create Sun light
	glPushMatrix();
	glTranslatef(sunX, sunY, sunZ);
	glColor3f(.8, .6, .2);
	glutSolidSphere(10, 50, 50);
	glPopMatrix();
	glEnable(GL_LIGHT5);
	SetPointLight(GL_LIGHT5, sunX, sunY, sunZ, .8, .6, .2);

	//create red light for traffic signal light
	glDisable(GL_LIGHT2);
	glPushMatrix();
	glTranslatef(-40, 5, 30);
	glColor3f(1, 0, 0);
	glutSolidSphere(.25, 100, 100);
	glPopMatrix();
	if (Time < 22 && LightTraffic) {
		glEnable(GL_LIGHT2);
		SetPointLight(GL_LIGHT2, -40, 5, 30, 1, 0, 0);
		cout << "Red light on " << endl;
	}
	else {
		glDisable(GL_LIGHT2);
	}

	//create green light for traffic signal light
	glDisable(GL_LIGHT3);
	glPushMatrix();
	glTranslatef(-40, 3, 30);
	glColor3f(0, 1, 0);
	glutSolidSphere(.25, 100, 100);
	glPopMatrix();
	if (Time < 66 && Time > 22 && LightTraffic) {
		glEnable(GL_LIGHT3);
		SetPointLight(GL_LIGHT3, -40, 3, 30, 0, 1, 0);
		cout << "Green light on" << endl;
	}
	else {
		glDisable(GL_LIGHT3);
	}

	//create yellow light for traffic signal light
	glDisable(GL_LIGHT4);
	glPushMatrix();
	glTranslatef(-40, 4, 30);
	glColor3f(.8, .9, .3);
	glutSolidSphere(.25, 100, 100);
	glPopMatrix();
	if (Time < 88 && Time > 66 && LightTraffic) {
		glEnable(GL_LIGHT4);
		SetPointLight(GL_LIGHT4, -40, 4, 30, .8, .9, 0.3);
		cout << "Yellow light on " << endl;
	}
	else {
		glDisable(GL_LIGHT4);
	}

	//create second light
	/*
	glPushMatrix();
	glTranslatef(xLight1, yLight1, zLight1);
	glColor3f(1, 1.0, 1.0);
	glutSolidSphere(.1, 50, 50);
	glPopMatrix();
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	/*
	if (Light1On) {
		glEnable(GL_LIGHT1);
		SetPointLight(GL_LIGHT1, xLight1, yLight1, zLight1, 1, 1, 1);
	}*/

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);

	//draw the person
	glPushMatrix();
	glTranslatef(ipersonX, .25, ipersonZ);
	glCallList(PersonList);
	glPopMatrix();
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	SetPointLight(GL_LIGHT0, ipersonX, .25, ipersonZ, .8, .7, .4);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);


	//blades
	glPushMatrix();
	glTranslatef(helicopterX, 2.9 + helicopterY, -2. + helicopterZ);
	glRotatef(BladeAngle, 0., 1., 0.);
	glRotatef(90.0, 1., 0., 0.);
	glScalef(5., 5., 5.);
	glCallList(BladeList);
	glPopMatrix();


	/*
	glPushMatrix();
	//glTranslatef(helicopterX, helicopterY, helicopterZ);
	glCallList(HeliList);
	glPopMatrix();*/

	/*
	//create blue point light (SHOULD BE ATTACHED TO CUBE)
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glPushMatrix();
	glTranslatef(light2x, light2y, light2z);
	//glTranslatef(0, 0, -1.);
	glColor3f(0.0, 0.0, 1.0);
	glutSolidSphere(.1, 50, 50);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	SetPointLight(GL_LIGHT1, light2x, light2y, light2z, 0, 0, 1);*/

	/*SetMaterial(1., 0., 0., 30);
	glutSolidCube(.4);*/
	//SetPointLight(2, 2, 0, 0, 0, 0, 1);
	

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	/*glColor3f(1., 0., 0. );
	glutSolidCube(.4);*/
	// draw the current object:

	/*
	glCallList( BoxList );

	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.,   0., 1., 0. );
			glCallList( BoxList );
		glPopMatrix( );
	}
	*/



	// draw some gratuitous text that just rotates on top of the scene:

	glDisable( GL_DEPTH_TEST );
	glColor3f( 0., 1., 1. );
	//DoRasterString( 0., 1., 0., "Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	//DoRasterString( 5., 5., 0., "Text That Doesn't" );


	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoViewMenu(int id) {
	viewMode = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int viewmenu = glutCreateMenu(DoViewMenu);
	glutAddMenuEntry("Bird's-eye", 0);
	glutAddMenuEntry("Person view", 1);

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu("View menu", viewmenu);
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	float innerRadius = .5, outerRadius = 1, nsides = 30, nrings = 30;
	glutSetWindow( MainWindow );
	float heliZ = -10;
	float r, g, b = 0;
	// create the object:
	//HELICOPTER
	HeliList = glGenLists(1);
	glNewList(HeliList, GL_COMPILE);
	glColor3f(0., 1., 0.);
	int i;
	struct edge *ep;
	struct point2 *p0, *p1;

	glPushMatrix();
	glTranslatef(0., -1., 0.);
	glRotatef(97., 0., 1., 0.);
	glRotatef(-15., 0., 0., 1.);
	glBegin(GL_LINES);
	for (i = 0, ep = Heliedges; i < Helinedges; i++, ep++)
	{
		p0 = &Helipoints[ep->p0];
		p1 = &Helipoints[ep->p1];
		glVertex3f(p0->x, p0->y, p0->z);
		glVertex3f(p1->x, p1->y, p1->z);
	}
	
	glEnd();
	glPopMatrix();
	glEndList();
	

	//create the blades
	BladeList = glGenLists(1);
	glNewList(BladeList, GL_COMPILE);
	glColor3f(1., 0., 0.);

	glBegin(GL_TRIANGLES);
	glVertex2f(BLADE_RADIUS, BLADE_WIDTH / 2.);
	glVertex2f(0., 0.);
	glVertex2f(BLADE_RADIUS, -BLADE_WIDTH / 2.);

	glVertex2f(-BLADE_RADIUS, -BLADE_WIDTH / 2.);
	glVertex2f(0., 0.);
	glVertex2f(-BLADE_RADIUS, BLADE_WIDTH / 2.);
	glEnd();
	glEndList();

	//Falcon
	FalconList = glGenLists(1);
	glNewList(FalconList, GL_COMPILE);
	glPushMatrix();
	
	glTranslatef(-10, 2.5, -40);
	glColor3f(1, 0, 0);
	LoadObjFile("car.obj");
	glPopMatrix();
	glEndList();

	//TorusList
	TorusList = glGenLists(1);
	glNewList(TorusList, GL_COMPILE);
	glutSolidTorus(innerRadius, outerRadius, nsides, nrings);
	glEndList();


	//grassy hill
	Grass = glGenLists(1);
	glNewList(Grass, GL_COMPILE);
	glPushMatrix();
	glTranslatef(30, -40, 110);
	int widthg, heightg;
	unsigned char *Textureg = BmpToTexture("grass.bmp", &widthg, &heightg);
	int levelg, ncompsg, borderg;
	levelg = 0;
	ncompsg = 3;
	borderg = 0;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, levelg, ncompsg, widthg, heightg, borderg, GL_RGB, GL_UNSIGNED_BYTE, Textureg);
	glEnable(GL_TEXTURE_2D);
	//glRotatef(360.*Time, 0., 1., 0.);
	MjbSphere(60, 100, 100, 0, false);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEndList();

	EarthList = glGenLists(1);
	glNewList(EarthList, GL_COMPILE);
	//draw Earth
	glPushMatrix();
	glTranslatef(30, 0, -300);
	int width, height;
	unsigned char *Texture = BmpToTexture("worldtex.bmp", &width, &height);
	int level, ncomps, border;
	level = 0;
	ncomps = 3;
	border = 0;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, level, ncomps, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, Texture);
	glEnable(GL_TEXTURE_2D);
	//glRotatef(360.*Time, 0., 1., 0.);
	MjbSphere(130, 100, 100, 0, false);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEndList();

	//Draw Street List
	StreetList = glGenLists(1);
	glNewList(StreetList, GL_COMPILE);
	glColor3f(1, 1, 1);
	int widthsl, heightsl;
	unsigned char *Texturesl = BmpToTexture("Cobble.bmp", &widthsl, &heightsl);
	int levelsl, ncompssl, bordersl;
	levelsl = 0;
	ncompssl = 3;
	bordersl = 0;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, levelsl, ncompssl, widthsl, heightsl, bordersl, GL_RGB, GL_UNSIGNED_BYTE, Texturesl);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glNormal3f(0., 0., 1.);

	glTexCoord2f(0, 0);
	glVertex3f(-80, 0, -80);
	glTexCoord2f(1, 0);
	glVertex3f(120, 0, -80);
	glTexCoord2f(1, 1);
	glVertex3f(120, 0, 80);
	glTexCoord2f(0, 1);
	glVertex3f(-80, 0, 80);


	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEndList();

	//Draw Wall List
	WallList = glGenLists(1);
	glNewList(WallList, GL_COMPILE);
	glPushMatrix();
	//glTranslatef(0, 6, 0);
	int widthw, heightw;
	unsigned char *Texturew = BmpToTexture("wall.bmp", &widthw, &heightw);
	int levelw, ncompsw, borderw;
	levelw = 0;
	ncompsw = 3;
	borderw = 0;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, levelw, ncompsw, widthw, heightw, borderw, GL_RGB, GL_UNSIGNED_BYTE, Texturew);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glNormal3f(0., 0., 1.);

	glTexCoord2f(0, 0);
	glVertex3f(120, 0, -80);
	glTexCoord2f(1, 0);
	glVertex3f(120, 10, -80);
	glTexCoord2f(1, 1);
	glVertex3f(120, 10, 80);
	glTexCoord2f(0, 1);
	glVertex3f(120, 0, 80);


	glEnd();

	glBegin(GL_QUADS);

	glNormal3f(0., 0., 1.);

	glTexCoord2f(0, 0);
	glVertex3f(-80, 0, -80);
	glTexCoord2f(1, 0);
	glVertex3f(-80, 10, -80);
	glTexCoord2f(1, 1);
	glVertex3f(-80, 10, 80);
	glTexCoord2f(0, 1);
	glVertex3f(-80, 0, 80);


	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	glEndList();

	//Draw Person
	PersonList = glGenLists(1);
	glNewList(PersonList, GL_COMPILE);
	glPushMatrix();
	glColor3f(.8, .7, .4);
	MjbSphere(.25, 100, 100, 0, false);
	glPopMatrix();
	glEndList();

	//Draw glassBuilding
	BuildingList = glGenLists(1);
	glNewList(BuildingList, GL_COMPILE);

	int widthgb, heightgb;
	unsigned char *Texturegb = BmpToTexture("glassBuilding.bmp", &widthgb, &heightgb);
	int levelgb, ncompsgb, bordergb;
	levelgb = 0;
	ncompsgb = 3;
	bordergb = 0;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, levelgb, ncompsgb, widthgb, heightgb, bordergb, GL_RGB, GL_UNSIGNED_BYTE, Texturegb);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glNormal3f(0., 0., 1.);

	glTexCoord2f(0, 0);
	glVertex3f(-24, 0, -24);
	glTexCoord2f(0, 1);
	glVertex3f(-24, 28, -24);
	glTexCoord2f(1, 1);
	glVertex3f(-24, 28, 24);
	glTexCoord2f(1, 0);
	glVertex3f(-24, 0, 24);


	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0., 0., 1.);
	glTexCoord2f(0, 0);
	glVertex3f(-48, 0, -24);
	glTexCoord2f(0, 1);
	glVertex3f(-48, 28, -24);
	glTexCoord2f(1, 1);
	glVertex3f(-48, 28, 24);
	glTexCoord2f(1, 0);
	glVertex3f(-48, 0, 24);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0., 0., 1.);
	glTexCoord2f(0, 0);
	glVertex3f(-48, 0, -24);
	glTexCoord2f(0, 1);
	glVertex3f(-48, 28, -24);
	glTexCoord2f(1, 1);
	glVertex3f(-24, 28, -24);
	glTexCoord2f(1, 0);
	glVertex3f(-24, 0, -24);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0., 0., 1.);
	glTexCoord2f(0, 0);
	glVertex3f(-48, 0, 24);
	glTexCoord2f(0, 1);
	glVertex3f(-48, 28, 24);
	glTexCoord2f(1, 1);
	glVertex3f(-24, 28, 24);
	glTexCoord2f(1, 0);
	glVertex3f(-24, 0, 24);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0., 0., 1.);
	glTexCoord2f(0, 0);
	glVertex3f(-48, 28, 24);
	glTexCoord2f(0, 1);
	glVertex3f(-48, 28, -24);
	glTexCoord2f(1, 1);
	glVertex3f(-24, 28, -24);
	glTexCoord2f(1, 0);
	glVertex3f(-24, 28, 24);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0., 0., 1.);
	glTexCoord2f(0, 0);
	glVertex3f(-48, 0, 24);
	glTexCoord2f(0, 1);
	glVertex3f(-48, 0, -24);
	glTexCoord2f(1, 1);
	glVertex3f(-24, 0, -24);
	glTexCoord2f(1, 0);
	glVertex3f(-24, 0, 24);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEndList();

	BoxList = glGenLists( 1 );
	glNewList( BoxList, GL_COMPILE );

		glBegin( GL_QUADS );

			glColor3f( 0., 0., 1. );
			glNormal3f( 0., 0.,  1. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f( -dx,  dy,  dz );

			glNormal3f( 0., 0., -1. );
				glTexCoord2f( 0., 0. );
				glVertex3f( -dx, -dy, -dz );
				glTexCoord2f( 0., 1. );
				glVertex3f( -dx,  dy, -dz );
				glTexCoord2f( 1., 1. );
				glVertex3f(  dx,  dy, -dz );
				glTexCoord2f( 1., 0. );
				glVertex3f(  dx, -dy, -dz );

			glColor3f( 1., 0., 0. );
			glNormal3f(  1., 0., 0. );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f(  dx,  dy,  dz );

			glNormal3f( -1., 0., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f( -dx,  dy, -dz );
				glVertex3f( -dx, -dy, -dz );

			glColor3f( 0., 1., 0. );
			glNormal3f( 0.,  1., 0. );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f( -dx,  dy, -dz );

			glNormal3f( 0., -1., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx, -dy, -dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx, -dy,  dz );

		glEnd( );

	glEndList( );


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:


void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;
		case '0':
			Light0On = !Light0On;
			cout << "Light0ON: " << Light0On << endl;
			break;

		case '1':
			Light1On = !Light1On;
			cout << "Light1ON: " << Light1On << endl;
			break;
		case '2':
			cout << "Light2ON: " << Light2On << endl;
			Light2On = !Light2On;
			break;
		case '6':
			cout << "Traffic Lights ON: " << LightTraffic << endl;
			LightTraffic = !LightTraffic;
			break;
		case 'f':
		case 'F':
			Freeze = !Freeze;
			if (Freeze)
				glutIdleFunc(NULL);
			else
				glutIdleFunc(Animate);
			break;
		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );


	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;

		case 1:
			r = q;	g = v;	b = p;
			break;

		case 2:
			r = p;	g = v;	b = t;
			break;

		case 3:
			r = p;	g = q;	b = v;
			break;

		case 4:
			r = t;	g = p;	b = v;
			break;

		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
