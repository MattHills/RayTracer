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
#include <FreeImage.h>
#include <time.h>
#include <math.h>

/*	This program allows you to see the convex hull of points on the screen as well as the hull peel for the points and the minimum spanning tree for the points.
	The mouse input works, but looses precision the on the screen. This was a problem multiple people had, and we were unsure why.
	I would restart the program everytime you run. Or randomize it after every command you click.

	The MST has 1 point that cycles, I could not find out why/where that error is. I believe it is in my sorting algorithm and not my MST algorithm.

*/

//Dynamic structures have a size to keep track of how much space was allocated, and a used to keep track of how much space was used up

// Structure used to traverse for MST to check for edges

// How many frames it survived
typedef struct{
	int frames;
} Age;

// To do;
typedef struct {
	int foo;
} State;

// Between 0-255
typedef struct{
	int r;
	int g;
	int b;
} Colour;

typedef struct {
	int scale;
} Scale;

// Between 0-359
typedef struct {
	int xAngle;
	int yAngle;
	int zAngle;
} Rotation;

typedef struct {
	float speed;
} Speed;

typedef struct {
	int x;
	int y;
	int z;
} Direction;

typedef struct {
	int x;
	int y;
	int z;
} Position;

typedef struct {
	int id;
	Position pos;
	Direction dir;
	Speed spe;
	Rotation rot;
	Scale sca;
	Colour col;
	State sta;
	Age age;
} Particle;

typedef struct{
	Particle *p;
	size_t size;
	size_t used;
} PartlicleList;


// Global Variables
int NUM_OF_PARTICLES;			// Number of points
int screenWidth;	// size * cellSize
int PLANE_SIZE;		// Number of edges in the minimum spanning tree
PartlicleList particles; //structure that holds the points on the MST
PartlicleList tempParticles;

void initParticleListStruct(PartlicleList *a, int size){
	a->p = (Particle *)malloc(size * sizeof(Particle));
	a->size = size;
	a->used = 0;
}

void insertParticleListStruct(PartlicleList *a, Particle p){
	if(a->used == a->size){
		a->size *= 2;
		a->p = (Particle *)realloc(a->p,a->size * sizeof(Particle));
	}
	a->p[a->used++] = p;
}

void freePartcileListStruct(PartlicleList *a) {
  free(a->p);
  a->p = NULL;
  a->used = a->size = 0;
}

void particleListCopy(PartlicleList *toCopy, PartlicleList *newCopy){
	int i;

	freePartcileListStruct(newCopy);
	initParticleListStruct(newCopy, toCopy->used);

	for(i = 0; i < toCopy->used; i++){
		insertParticleListStruct(newCopy, toCopy->p[i]);
	}	
}

void redraw(){
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	  
	//glColor3f(0.0,0.0,1.0);
	//glBegin(GL_QUADS);
	//glVertex3f(-PLANE_SIZE, 0 , -PLANE_SIZE);
	//glVertex3f(PLANE_SIZE, 0 , -PLANE_SIZE);
	//glVertex3f(PLANE_SIZE, 0 , PLANE_SIZE);
	//glVertex3f(-PLANE_SIZE, 0 , PLANE_SIZE);
	//glEnd();

	glColor3f(1.0,1.0,1.0);
	glLoadIdentity();
	glScalef(1.0,2.0,1.0);
	glutWireCube(1.0);
	//glutSolidCube(1000);
	//glutSolidCone(200,100,100,100);
	glPopMatrix();

	glutSwapBuffers();
	glFlush();

}

// Changes the window size
void reshape(int x, int y) {

   glViewport(0, 0, x, y);
   glClear(GL_COLOR_BUFFER_BIT);
   glFlush();
}
// Resets the screen, not very well though
void reset(){
	particleListCopy(&particles, &tempParticles);
	freePartcileListStruct(&particles);
	initParticleListStruct(&particles, tempParticles.used);
	
	glutPostRedisplay();
}


// Keyboard imput
void keyboard(unsigned char key, int x, int y) {

	switch (key){
		case 0x1B:
		case'q':
		case 'Q':
			exit(0);
			break;
		case '1':
			 //live();
			break;
		case 'r':
		case 'R':
			reset();
			break;
		case 'k':
		case 'K':			
			//startKruskalsNew();
			//startKruskals();
			break;
		case 'm':
		case 'M':
			//toggleMouseEditMode();
			break;
		case 'f':
		case 'F':
			//funPeel(0);
			break;
		case 'h':
		case 'H':
			//hullPeel();
			break;
		case 'a':
		case 'A':
			//randomizeGrid();
			break;
		case 'c':
		case 'C':
			//beginConvexHullStart();			
			break;
		case 'i':
		case 'I':
			//insert();
			break;
   }
}


main(int argc, char **argv)
{
	int i;
	// Initialize game settings
	NUM_OF_PARTICLES = 100;
	PLANE_SIZE = 100;
	screenWidth = 500;

	initParticleListStruct(&particles, NUM_OF_PARTICLES);
	
	printf("Q:quit\nR:reset\n");
    glutInit(&argc, argv);
    glutInitWindowSize(screenWidth, screenWidth);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Particle Fountain");

   // glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(redraw);
    glutReshapeFunc(reshape);

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(20, screenWidth/screenWidth, 0.1, 1500);
	gluLookAt(900, 250, 800, 0, 0, 0, 0, 1, 0);


	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	vect.
    glutMainLoop();

}