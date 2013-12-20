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
	Angle ang
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
typedef struct Rectangle {
	Position pos;	
	Size siz;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Rectangle *next;
} Rectangle;

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
	struct Rectangle *rec;
	struct Sphere *sph;
	struct Plane *pla;
	struct Donut *don;
} glob;
glob global;

int screenWidth;	// size * cellSize

void redraw(){
	int i;
	Particle* p;
	double size;

	p = headParticle;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	if(culling == 1){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else{
		glDisable(GL_CULL_FACE);
	}

	if(colourMode == 0){
		glColor3f(1.0,1.0,1.0);
	}
	if(p != 0 && p->sta.alive == 0){
		headState = deleteHeadParticle();
		p = headParticle;
	}
	while(p && headState == 1){
		//printf("%d \n", p.pos.y);
		moveParticle(p);
		//p = particles.p[i];
		detectCollision(p);
		glPushMatrix();		
		// Move particle
		glTranslatef(p->pos.x, p->pos.y, p->pos.z);
		if(rotate == 1){
			glRotatef(p->rot.angle,p->rot.xAxis,p->rot.yAxis,p->rot.zAxis);
		}
		
		if(colourMode == 1){
			glColor3f(p->col.r,p->col.g,p->col.b);
		}

		if(randSize == 1){
			size = p->sca.scale;
		}
		else{
			size = 5;
		}
		if(viewMode == 0){
			glutSolidCube(size);
		}
		else if(viewMode == 1){
			glutWireCube(size);
		}
		else{
			glBegin( GL_POINTS);
				glVertex3f(p->pos.x, p->pos.y, p->pos.z);
			glEnd();
		}
		glPopMatrix();		
		// Check if next particle is alive before selecting it
		if(p->next !=0 && p->next->sta.alive == 0){
			deleteParticle(p);
		}		
		p = p->next;
	}
	free(p);
	  
	glColor3f(0.,0.0,1.0);
	if(viewMode == 0){
		glBegin(GL_QUADS);
			glVertex3f(-PLANE_SIZE, 0 , PLANE_SIZE);
			glVertex3f(PLANE_SIZE, 0 , PLANE_SIZE);
			glVertex3f(PLANE_SIZE, 0 , -PLANE_SIZE);
			glVertex3f(-PLANE_SIZE, 0 , -PLANE_SIZE);			
		glEnd();
	}
	else{
		glBegin(GL_LINES);
			glVertex3f(PLANE_SIZE, 0, PLANE_SIZE);
			glVertex3f(PLANE_SIZE, 0, -PLANE_SIZE);
			glVertex3f(PLANE_SIZE, 0, -PLANE_SIZE);
			glVertex3f(-PLANE_SIZE, 0, -PLANE_SIZE);
			glVertex3f(-PLANE_SIZE, 0, -PLANE_SIZE);
			glVertex3f(-PLANE_SIZE, 0, PLANE_SIZE);
			glVertex3f(-PLANE_SIZE, 0, PLANE_SIZE);
			glVertex3f(PLANE_SIZE, 0, PLANE_SIZE);
		glEnd();
	}
	if(viewMode == 0){
		glColor3f(0.0,0.0,0.0);
		glBegin(GL_QUADS);
			glVertex3f(HOLE_MAX, 1 , HOLE_MIN);
			glVertex3f(HOLE_MIN, 1 , HOLE_MIN);
			glVertex3f(HOLE_MIN, 1 , HOLE_MAX);
			glVertex3f(HOLE_MAX, 1 , HOLE_MAX);			
		glEnd();
	}
	else{
		glColor3f(0.0,1.0,0.0);
		glBegin(GL_LINES);
			glVertex3f(HOLE_MAX, 1 , HOLE_MAX);
			glVertex3f(HOLE_MAX, 1 , HOLE_MIN);
			glVertex3f(HOLE_MAX, 1 , HOLE_MIN);
			glVertex3f(HOLE_MIN, 1 , HOLE_MIN);
			glVertex3f(HOLE_MIN, 1 , HOLE_MIN);
			glVertex3f(HOLE_MIN, 1 , HOLE_MAX);
			glVertex3f(HOLE_MIN, 1 , HOLE_MAX);
			glVertex3f(HOLE_MAX, 1 , HOLE_MAX);
		glEnd();
	}

	glPushMatrix();
	glRotatef(270,1,0,0);
	glColor3f(0.5,0.5,0.5);
	if(viewMode == 0){		
		gluCylinder(gluNewQuadric(),5,5,40,40,40);		
	}
	else{
		glutWireCylinder(5,40,5,5);
	}
	
	glPopMatrix();	

	if(spinAxis == 0){
		glRotatef(angle, 0.0, 0.0, 1.0);
	}
	else if(spinAxis == 1){
		glRotatef(angle, 1.0, 0.0, 0.0);
	}
	else if(spinAxis == 2){
		glRotatef(angle, 0.0, 1.0, 0.0);
	}
	
	glFlush();
	glutSwapBuffers();
	

}

// Changes the window size
void reshape(int x, int y) {
   glViewport(0, 0, x, y);
   glClear(GL_COLOR_BUFFER_BIT);
   glFlush();
}

// Resets the screen, not very well though
void reset(){
	fireMode = 0;
	rotate = 0;
	friction = 0;
	viewMode = 0;
	spinAxis = 2;
	culling = 0;
	particleSpeed = 0;
	colourMode = 0;
	randSize = 0;
	angle = 0;
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
		case 'p':
		case 'P':
			setViewPoly();
			break;
		case 'o':
		case 'O':
			setWireFrame();
			break;
		case 'i':
		case 'I':
			setPoints();
			break;
		case 'a':
		case 'A':
			 toggleFireMode();
			break;
		case 'r':
		case 'R':
			reset();
			break;
		case 's':
		case 'S':			
			setSpeed();
			break;
		case 'b':
		case 'B':
			toggleRotate();
			break;
		case 'n':
		case 'N':
			toggleFriction();
			break;
		case 'f':
		case 'F':
			toggleFireMode();
			break;
		case 'z':
		case 'Z':
			setSpin(0);
			break;
		case 'x':
		case 'X':
			setSpin(1);
			break;
		case 'y':
		case 'Y':
			setSpin(2);
			break;
		case 'c':
		case 'C':
			toggleCulling();
			break;
		case 'v':
		case 'V':
			toggleColours();
			break;
		case 'd':
		case 'D':
			toggleSize();
			break;
		case 'u':
		case 'U':
			setSpray();
			break;
   }
}

void readFile(){
	FILE *file;
	int i;
	Sphere s;

	file = fopen("raydetails.txt","r");

	if(file == NULL){
		printf("Error when opening file");
	}
	else{
		fscanf(file, "%d", &i);
		while(!feof(file)){
			printf("%d",i);
			fscanf(file, "%d", &i);
		}
	}
	fclose(file);



}


main(int argc, char **argv)
{
	// Initialize game settings
	screenWidth = 500;	
	
	//printf("Q:quit\nU:toggle spray\nZ:spin on z\nX:spin on x\nY:spin on y\nC:toggle culling\nV:toggle colours\nD:toggle size\nI:view points\nO:view wire frame\nP:view polygons\nA:toggle auto fire particles\nS:set speed\nF:manual fire mode\nB:toggle rotate\nN:toggle friction\nR:reset\n");
    glutInit(&argc, argv);
    glutInitWindowSize(screenWidth, screenWidth);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	
    glutCreateWindow("Particle Fountain");

    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyBoardUp);
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

	autoFireParticles(0);

    glutMainLoop();
	

}