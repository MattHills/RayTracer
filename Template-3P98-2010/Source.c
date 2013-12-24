#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <freeglut.h>
#include <time.h>
#include <math.h>

#define MAX_RAY_DEPTH 3

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


typedef struct Vector {
	//Vector values
	//still need to normalize
   float x, y, z;
   float magnitude;
   float normalize;
}Vector;

//mag (sqrt((x*x) + (y*y) + (z*z))
//normalize = 
//invert negative -x,-y,-z
//dotproduct (vector v) return 
//crossproduct

typedef struct Ray{
	struct Vector origin;
	struct Vector direction;
} Ray;

typedef struct Camera{
	//Coord for scene for perspective
	struct Vector campos;
	struct Vector campdir;
	struct Vector camright;
	struct Vector camdown;
} Camera;

typedef struct Color {
	float r;
	float g;
	float b;
} Color;

typedef struct Light{
	struct Color c;//color
	struct Vector v;//position
} Light;

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


//Vector functions

double magnitude (Vector v){
	return (sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z)));
}//returns magnitude of a vector

Vector normalize (Vector v){
	v.x = v.x/magnitude(v);
	v.y = v.y/magnitude(v);
	v.z = v.z/magnitude(v);
	return v;
}//normalize a vector

Vector negative (Vector v){
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	return v;
}//negative of a vector

double dotProduct(Vector v, Vector c){
	return (v.x*c.x + v.y*c.y + v.z*c.z);
}//dotProdcut of two vectors v and c

Vector crossProd(Vector v, Vector c){
	v.x = (v.y*c.z-v.z*c.y);
	v.y = (v.z*c.x-v.x*c.z);
	v.z = (v.x*c.y-v.y*c.x);
	return v;
}//returns the cross prod of two vectors


void addVectors(Vector v, Vector c){
	v.x = v.x+c.x;
	v.y = v.y+c.y;
	v.z = v.z+c.z;
}//addtion of vector v and c
//sets Vector v's coord to the new vector created

void multVectors(Vector v, double scalar){
	v.x = v.x*scalar;
	v.y = v.y*scalar;
	v.z = v.z*scalar;
}//multiple of vector v and scalar value
//sets Vector v's coord to the new vector created


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

	//Ray declaration stuff
	int i,j;
	int x,y,z;
	Camera camera;
	Vector X,Y,Z;
	Vector look_at;
	Vector diff_btw;
	Vector camdir;
	Vector camright;
	Vector camdown;
	

	Color white;
	Color green;
	Color black;

	Vector light_pos;
	Vector campos;
	Light scene_light;


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


	//Ray stuff	

	X.x = 1;
	X.y = 0;
	X.z = 0;

	Y.x = 0;
	Y.y = 1;
	Y.z = 0;
	
	Z.x = 0;
	Z.y = 0;
	Z.z = 1;

	//campos (3,.5,-4) above y plane

	campos.x = 3;
	campos.y = 1.5;
	campos.z = -4;

	look_at.x = 0;
	look_at.y = 0;
	look_at.z = 0;

	diff_btw.x = campos.x-look_at.x;
	diff_btw.y = campos.y-look_at.y;
	diff_btw.z = campos.z-look_at.z;

	//camdir is diff_btw neg.normalize
	//camright y.crossprod(camdir).normalize
	//camdown camright.crossprod(camdir)
	//camera scene cam (campos, cam dir, right, camdown)

	camdir = normalize(negative(diff_btw));
	camright = normalize(crossProd(camdir,Y));
	camdown = crossProd(camdir,camright);

	camera.campos = campos;
	camera.camdown = camdir;
	camera.camright = camright;
	camera.campdir = camdir;


	light_pos.x = -7;
	light_pos.y = 10;
	light_pos.z = -10;


	//Basic Light declarations
	white.r = 1.0;
	white.g = 1.0;
	white.b = 1.0;

	green.r = 0.5;
	green.g = 1.0;
	green.b = 0.5;

	black.r = 0.0;
	black.g = 0.0;
	black.b = 0.0;


	//Scene light is white with vector or position
	scene_light.c = white;
	scene_light.v = light_pos;

	for (i = 0;i<screenWidth;i++){
		for (j = 0;j<screenWidth;j++){		
			//direction of ray
			
		}
	}



	//



	//readFile();
	//drawShapes();
    glutMainLoop();
	

}