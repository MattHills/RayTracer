/*	Matt Hills
	mh09wq
	4640512
	Friday, November 8, 2013
	COSC 3P98
	Assignment 2 Question 1 and 4
	2D Convex Hull, MST
*/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <freeglut.h>
#include <time.h>
#include <math.h>

/*	This program allows you to see the convex hull of points on the screen as well as the hull peel for the points and the minimum spanning tree for the points.
	The mouse input works, but looses precision the on the screen. This was a problem multiple people had, and we were unsure why.
	I would restart the program everytime you run. Or randomize it after every command you click.

	The MST has 1 point that cycles, I could not find out why/where that error is. I believe it is in my sorting algorithm and not my MST algorithm.

*/

typedef struct {
	float val;
} Transparency;

// To angle on the shapes
typedef struct {
	float x;
	float y;
	float z;
} Angle;

// Between 0-255
typedef struct {
	float r;
	float g;
	float b;
} Colour;

// Size of shape
typedef struct {
	float height;
	float width;
	float depth;
} Size;

// Position of shape
typedef struct {
	float x;
	float y;
	float z;
} Position;

// Radius for circular shapes
typedef struct {
	float internalRadius;
	float totalRadius;
} Radius;

// Torus shape
typedef struct Donut {
	Position pos;
	Radius rad;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Donut *next;
} Donut;

// Plane shape
typedef struct Plane {
	Position pos;
	Size siz;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Plane *next;
} Plane;

// Sphere shape
typedef struct Sphere {
	Position pos;	
	Radius rad;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Sphere *next;
} Sphere;

// Rectangle shape
typedef struct Rect {
	Position pos;	
	Size siz;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Rect *next;
} Rect;

// Cylinder shape
typedef struct Cylinder{
	Position pos;	
	Size size;
	Radius rad;	
	Angle ang;
	Colour col;
	Transparency trans;
	struct Cylinder *next;
} Cylinder;

typedef struct Triangle {
	Position pos;
	Size size;
	Angle ang;
	Colour col;	
	Transparency trans;
	struct Triangle *next;
} Triangle;

//the global structure
typedef struct {
	struct Cylinder *cyl;
	struct Rect *rec;
	struct Sphere *sph;
	struct Plane *pla;
	struct Donut *don;
} glob;
glob global;

int screenWidth;	// size * cellSize

void redraw(){

}

// Changes the window size
void reshape(int x, int y) {
   glViewport(0, 0, x, y);
   glClear(GL_COLOR_BUFFER_BIT);
   glFlush();
}

// Resets the screen, not very well though
void reset(){
	glPopMatrix();
	glPushMatrix();
}

// Keyboard imput
void keyboard(unsigned char key, int x, int y) {

	switch (key){
		case 0x1B:
		case'q':
		case 'Q':
			exit(0);
			break;
   }
}

void readFile(){
	FILE *file;
	float i;
	Sphere s;

	//file = fopen("C:\3P98\3P98 Final Project\Ray Tracer\RayTracer\raydetails.txt","r");
	file = fopen("raydetails.txt","r");

	if(!file){
		printf("Error when opening file");
	}
	else{
		fscanf(file, "%f", &i);
		while(!feof(file)){
			if(i == 0){
				fscanf(file, "%f", &i);
				s.pos.x = i;
				fscanf(file, "%f", &i);
				s.pos.y = i;
				fscanf(file, "%f", &i);
				s.pos.z = i;
				fscanf(file, "%f", &i);
				s.rad.totalRadius = i;				
				s.ang.x = 0;
				s.ang.y = 0;
				s.ang.z = 0;
				fscanf(file, "%f", &i);
				s.col.r = i;
				fscanf(file, "%f", &i);
				s.col.b = i;
				fscanf(file, "%f", &i);
				s.col.g = i;
				fscanf(file, "%f", &i);
				s.trans.val = i;
				s.next = (Sphere*)malloc(sizeof (struct Sphere));

				if(!global.sph){
					global.sph = (Sphere*)malloc(sizeof (struct Sphere));
					global.sph = &s;
				}
				else{
					s.next = global.sph;
					global.sph = &s;
				}
			}
			fscanf(file, "%f", &i);
		}
		fclose(file);
	}	
}

/*
ray details sheet
type: 
x pos:
y pos:
z pos:
width:
height:
depth:
radius:
angle x:
angle y:
angle z:
colour r:
colour g:
colour b:
transparency:


*/

void drawShapes(){

}


main(int argc, char **argv)
{
	// Initialize game settings
	screenWidth = 500;	
	
	//printf("Q:quit\nU:toggle spray\nZ:spin on z\nX:spin on x\nY:spin on y\nC:toggle culling\nV:toggle colours\nD:toggle size\nI:view points\nO:view wire frame\nP:view polygons\nA:toggle auto fire particles\nS:set speed\nF:manual fire mode\nB:toggle rotate\nN:toggle friction\nR:reset\n");
    glutInit(&argc, argv);
    glutInitWindowSize(screenWidth, screenWidth);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	
    glutCreateWindow("Ray Tracer");

    glutKeyboardFunc(keyboard);
    glutDisplayFunc(redraw);
	glutIdleFunc(redraw);
    glutReshapeFunc(reshape);

    glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glLoadIdentity();

	gluPerspective(20, screenWidth/screenWidth, 0.1, 1500);
	gluLookAt(900, 250, 800, 0, 0, 0, 0, 1, 0);


	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glPushMatrix();

	readFile();

	drawShapes();

    glutMainLoop();
	

}