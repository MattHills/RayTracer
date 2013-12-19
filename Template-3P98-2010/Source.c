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

// How many frames it survived
typedef struct{
	int frames;
} Age;

// To do;
typedef struct {
	int alive;
} State;

// Between 0-255
typedef struct{
	float r;
	float g;
	float b;
} Colour;

typedef struct {
	double scale;
} Scale;

// Between 0-359
typedef struct {
	float xAxis;
	float yAxis;
	float zAxis;
	int angle;
	float angleFactor;
} Rotation;

typedef struct {
	float x;
	float y;
	float z;
} Speed;

typedef struct {
	float x;
	float y;
	float z;
} Direction;

typedef struct {
	float x;
	float y;
	float z;
} Position;

typedef struct Particle {
	Position pos;
	Direction dir;
	Speed spe;
	Rotation rot;
	Scale sca;
	Colour col;
	State sta;
	Age age;
	struct Particle *next;
} Particle;

//the global structure
typedef struct {
	Particle dude;
} glob;
glob global;


// Global Variables
int NUM_OF_PARTICLES;			// Number of points
int particleCount;
int fireMode;
int rotate;
int friction;
int viewMode;
int spinAxis;
int culling;
int particleSpeed;
int colourMode;
int randSize;
int headState;
int sprayState;
int screenWidth;	// size * cellSize
int PLANE_SIZE;		// Number of edges in the minimum spanning tree
int HOLE_MAX;
int HOLE_MIN;
int POLY_WIDTH;
int POLY_HEIGHT;
int POLY_START;
double CONE_BASE = 50;
double CONE_HEIGHT = 40;

float angle=0.0;

Particle *headParticle;

int deleteHeadParticle(){
	Particle* newHead;
	int headState;

	if(headParticle->next != 0){
		 newHead = (Particle*)malloc(sizeof (struct Particle));
		*newHead = *headParticle->next;
		free(headParticle);
		headParticle = (Particle*)malloc(sizeof (struct Particle));
		*headParticle = *newHead;
		headState = 1;
		free(newHead);
	}
	else{
		free(headParticle);
		headParticle = NULL;
		headState = 0;
	}
	particleCount--;
	return headState;
}

void deleteParticle(Particle *parent){
	Particle* particle;
	Particle* next;

	particle = (Particle*)malloc(sizeof (struct Particle));
	next = (Particle*)malloc(sizeof (struct Particle));

	if(parent->next->next != 0){
		*particle = *parent->next;
		*next = *particle->next;
		*parent->next = *next;
	}
	else{
		free(parent->next);
		parent->next = (Particle*)malloc(sizeof (struct Particle));
		parent->next = 0;
	}
	particleCount--;
	free(particle);
	free(next);
}

void createParticles(Particle *parent, int addNumP){
	Age age;
	State state;
	Colour colour;
	Scale scale;
	Rotation rotation;
	Speed speed;
	Direction direction;
	Position position;
	Particle* particle;
	double iX, iY, iZ;
	double diX, diY, diZ;
	double rX, rY, rZ;

	if(particleCount <= 250){

		particle = (Particle*)malloc(sizeof (struct Particle));

		age.frames = 100;

		state.alive = 1;

		colour.b = (float) rand() / (float)RAND_MAX;
		colour.g = (float) rand() / (float)RAND_MAX;
		colour.r = (float) rand() / (float)RAND_MAX;

		scale.scale = (double)rand() * 10 / (double)RAND_MAX + 10;		

		srand(__rdtsc());

		rotation.xAxis = (float)rand() / (float)RAND_MAX;
		rotation.yAxis = (float)rand() / (float)RAND_MAX;
		rotation.zAxis = (float)rand() / (float)RAND_MAX;
		rotation.angle = (int)rand() * 10  / (int)RAND_MAX + 10;
		rotation.angleFactor = (float)rand() / (float)RAND_MAX;
		rotation.angleFactor *= 16;
		rotation.angleFactor -= 8;

		if(rotation.angleFactor < 0){
			rotation.angle *= -1;
		}
	
		if(particleSpeed != 1){
			iX = rand() % 2 + 0.1;
			if(sprayState == 1){
				iY = 1;
			}
			else{
				iY = rand() % 3;
			}
			iZ = rand() % 2 + 0.1;	
		
			if(iY < 2){
				iX+=1;
				iZ+=1;
			}
		}
		else{
			iX = 1;
			if(sprayState == 1){
				iY = 2;
			}
			else{
				iY = 1;
			}
			iZ = 1;
		}	
		
		diX = (float)rand() / (float)RAND_MAX ;
		diX *= 4;
		diX -= 2;
		
		diY = (float)rand() / (float)RAND_MAX;		
		diY *= 2;

		diZ = (float)rand() / (float)RAND_MAX;		
		diZ *= 4;
		diZ -= 2;

		direction.x = diX;
		direction.y = diY;
		direction.z = diZ;

		speed.x = iX;
		speed.y = iY;
		speed.z = iZ;

		position.x = 0;
		position.y = 40;
		position.z = 0;

		particle->pos = position;
		particle->dir = direction;
		particle->spe = speed;
		particle->rot = rotation;
		particle->sca = scale;
		particle->col = colour;
		particle->sta = state;
		particle->age = age;

		if(headState == 0){
			free(headParticle);
			headParticle = (Particle*)malloc(sizeof (struct Particle));
			*headParticle = *particle;
			headParticle->next = (Particle*)malloc(sizeof (struct Particle));
			headParticle->next = 0;
			headState = 1;
		}
		else{
			particle->next = (Particle*)malloc(sizeof (struct Particle));
			if(particleCount > 1){
				*particle->next = *headParticle->next;
			}
			else{
				particle->next = 0;
			}			
			free(headParticle->next);
			headParticle->next = (Particle*)malloc(sizeof (struct Particle));
			*headParticle->next = *particle;	
		}		
	
		particleCount++;
		
		if(addNumP > 1){
			createParticles(particle, addNumP--);
		}
		free(particle);
	}	
}

void moveParticle(Particle *p){
	float addX,addZ;
	float sx,sy,sz;
	Direction dir;
	int rot;
	float rotFact;
	float scale;
	
	dir = p->dir;

	sx = p->spe.x;
	sy = p->spe.y - 0.03;
	sz = p->spe.z;

	if(friction == 1){
		sx /= 1.1;
		sz /= 1.1;
	}

	if((sx <= 0.01 || sz <= 0.01)){
		p->sta.alive = 0;
	}
	

	p->pos.x += dir.x * sx;
	p->pos.y += dir.y * sy;
	p->pos.z += dir.z * sz;

	p->spe.y = sy;

	if(randSize == 1){
		scale = p->sca.scale;
	}
	else{
		scale = 1;
	}

	if(p->pos.y-scale <= -40){
		p->sta.alive = 0;
	}

	// FIX THIS
	rot = p->rot.angle;
	rotFact = p->rot.angleFactor;

	/*if(rot >= 360){
		rot = 0;
	}
	else if(rot <= -360){
		rot = 0;
	}*/

	rot += rotFact;	

	p->rot.angle = rot;

}

void detectCollision(Particle* p){
	Position pos;
	Direction dir;
	float addX,addZ;
	float sx,sy,sz;
	float scale;

	dir = p->dir;

	pos = p->pos;


	if(randSize == 1){
		scale = p->sca.scale;
	}
	else{
		scale = 1;
	}

	sx = p->spe.x;
	sy = p->spe.y - 0.03;
	sz = p->spe.z;

	if(friction == 1){
		sx /= 1.1;
		sz /= 1.1;
	}	

	pos.x += dir.x * sx;
	pos.y += dir.y * sy;
	pos.z += dir.z * sz;

	// Check if on the plane
	if(pos.y-scale <= 1){
		// Check Hole
		if(pos.x-scale >= HOLE_MIN && pos.x+scale <= HOLE_MAX && pos.z-scale >= HOLE_MIN && pos.z+scale <= HOLE_MAX){
		}
		// Inside Plane Boundaries
		else if(pos.x >= -PLANE_SIZE && pos.x <= PLANE_SIZE){
			if(pos.z >= -PLANE_SIZE & pos.z <= PLANE_SIZE){
				p->spe.y *= (-1) + 0.05;
			}
		}
	}
}

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

void mouse(int btn, int state, int x, int y){
	
	if(state == GLUT_DOWN){
		if(btn == GLUT_LEFT_BUTTON){
			angle = angle + 1.0;
		}
		else if(btn == GLUT_RIGHT_BUTTON){
			angle = angle - 1.0;
		}
		else{
			angle = 0.0;
		}
	}
}

void setSpeed(){
	if(1 == particleSpeed){
		particleSpeed = 10;
	}
	else{
		particleSpeed = 1;
	}
}

void fireParticles(int i){
	createParticles(headParticle,1);
}

void autoFireParticles(int i){
	if(fireMode == 0){
		createParticles(headParticle, 1);
		glutTimerFunc(200, autoFireParticles, 0);
	}
}

void toggleFireMode(){
	if(fireMode == 1){
		fireMode = 0;
		autoFireParticles(0);
	}
	else{
		fireMode = 1;
	}
}

void keyBoardUp(unsigned char key, int x, int y){
	switch(key){
		case 'f':
		case 'F':
			fireMode = 1;			
			break;
	}
}

void toggleRotate(){
	if(rotate == 1){
		rotate = 0;
	}
	else{
		rotate = 1;
	}
}

void toggleFriction(){
	if(friction == 1){
		friction = 0;
	}
	else{
		friction = 1;
	}
}

void setViewPoly(){
	viewMode = 0;
}

void setWireFrame(){
	viewMode = 1;
}

void setPoints(){
	viewMode = 2;
}

void setSpin(int i){
	spinAxis = i;
}

void toggleCulling(){
	if(culling == 0){
		culling = 1;
	}
	else{
		culling = 0;
	}
}

void toggleColours(){
	if(colourMode == 1){
		colourMode = 0;
	}
	else{
		colourMode = 1;
	}
}

void toggleSize(){
	if(randSize == 0){
		randSize = 1;
	}
	else {
		randSize = 0;
	}
}

void setSpray(){
	if(sprayState == 0){
		sprayState = 1;
	}
	else{
		sprayState = 0;
	}
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

	file = fopen("raydetails.txt","r");

	if(file == NULL){
		printf("Error when opening file");
	}
	else{
		while(!feof(file)){
			
		}
	}
	



}


main(int argc, char **argv)
{
	// Initialize game settings
	
	fireMode = 1;
	rotate = 0;
	friction = 1;
	viewMode = 0;
	spinAxis = 2;
	culling= 0;
	particleSpeed = 0;
	colourMode = 0;
	randSize = 0;
	sprayState = 0;
	POLY_START = -70;
	POLY_HEIGHT = 10;
	POLY_WIDTH = 60;

	NUM_OF_PARTICLES = 100;
	particleCount = 0;
	PLANE_SIZE = 150;
	HOLE_MAX = 100;
	HOLE_MIN = 50;
	CONE_BASE = 10;
	CONE_HEIGHT = 40;
	screenWidth = 500;	

	//initParticleListStruct(&particles, NUM_OF_PARTICLES);
	headParticle = (Particle*)malloc(sizeof (Particle));
	headParticle = 0;
	createParticles(headParticle, 1);
	//part = createParticle();
	//insertParticleListStruct(&particles, part);
	
	printf("Q:quit\nU:toggle spray\nZ:spin on z\nX:spin on x\nY:spin on y\nC:toggle culling\nV:toggle colours\nD:toggle size\nI:view points\nO:view wire frame\nP:view polygons\nA:toggle auto fire particles\nS:set speed\nF:manual fire mode\nB:toggle rotate\nN:toggle friction\nR:reset\n");
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