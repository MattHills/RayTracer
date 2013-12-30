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
	int id;
	Position pos;
	Radius rad;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Donut *next;
} Donut;

// Plane shape
typedef struct Plane {
	int id;
	Position pos;
	Size siz;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Plane *next;
} Plane;

// Sphere shape
typedef struct Sphere {
	int id;
	Position pos;	
	Radius rad;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Sphere *next;
} Sphere;

// Rectangle shape
typedef struct Rect {
	int id;
	Position pos;	
	Size siz;
	Angle ang;
	Colour col;
	Transparency trans;
	struct Rect *next;
} Rect;

// Cylinder shape
typedef struct Cylinder{
	int id;
	Position pos;	
	Size size;
	Radius rad;	
	Angle ang;
	Colour col;
	Transparency trans;
	struct Cylinder *next;
} Cylinder;

typedef struct Triangle {
	int id;
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

typedef struct Object{
	Color color;
	double findIntersection;
} Object;


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
	Sphere *s;

	//file = fopen("C:\3P98\3P98 Final Project\Ray Tracer\RayTracer\raydetails.txt","r");
	file = fopen("raydetails.txt","r");

	if(!file){
		printf("Error when opening file");
	}
	else{
		fscanf(file, "%f", &i);
		while(!feof(file)){
			if(i == 0){
				s = (Sphere*)malloc(sizeof (struct Sphere));
				fscanf(file, "%f", &i);
				s->pos.x = i;
				fscanf(file, "%f", &i);
				s->pos.y = i;
				fscanf(file, "%f", &i);
				s->pos.z = i;
				fscanf(file, "%f", &i);
				s->rad.totalRadius = i;				
				s->ang.x = 0;
				s->ang.y = 0;
				s->ang.z = 0;
				fscanf(file, "%f", &i);
				s->col.r = i;
				fscanf(file, "%f", &i);
				s->col.b = i;
				fscanf(file, "%f", &i);
				s->col.g = i;
				fscanf(file, "%f", &i);
				s->trans.val = i;
				s->next = (Sphere*)malloc(sizeof (struct Sphere));

				if(!global.sph){
					global.sph = (Sphere*)malloc(sizeof (struct Sphere));
					global.sph = s;
				}
				else{
					s->next = global.sph;
					global.sph = s;
				}
				//free(s);
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
	Vector temp;
	temp.x = v.x/magnitude(v);
	temp.y = v.y/magnitude(v);
	temp.z = v.z/magnitude(v);
	return temp;
}//normalize a vector

Vector negative (Vector v){
	Vector temp;
	temp.x = -v.x;
	temp.y = -v.y;
	temp.z = -v.z;
	return temp;
}//negative of a vector

double dotProduct(Vector v, Vector c){
	return (v.x*c.x + v.y*c.y + v.z*c.z);
}//dotProdcut of two vectors v and c

Vector crossProd(Vector v, Vector c){
	Vector temp;
	temp.x = (v.y*c.z-v.z*c.y);
	temp.y = (v.z*c.x-v.x*c.z);
	temp.z = (v.x*c.y-v.y*c.x);
	return temp;
}//returns the cross prod of two vectors


/*void addVectors(Vector v, Vector c){
	v.x = v.x+c.x;
	v.y = v.y+c.y;
	v.z = v.z+c.z;
}*/
//addtion of vector v and c
//sets Vector v's coord to the new vector created
Vector addVectors(Vector v, Vector c){
	Vector temp;
	temp.x = v.x+c.x;
	temp.y = v.y+c.y;
	temp.z = v.z+c.z;
	return temp;
}


/*void multVectors(Vector v, double scalar){
	v.x = v.x*scalar;
	v.y = v.y*scalar;
	v.z = v.z*scalar;
}*/
//multiple of vector v and scalar value
//sets Vector v's coord to the new vector created

Vector multVectors(Vector v, double scalar){
	Vector temp;
	temp.x = v.x*scalar;
	temp.y = v.y*scalar;
	temp.z = v.z*scalar;
	return temp;
}

//Plane intersection

/*
double findIntersection(Ray ray,Plane p){
	Vector ray_direction = ray.direction;

	double a = dotProduct(ray_direction,p.);

	if (a == 0){
	//ray is parrallel to plane
		return -1;
	}else{
		Vector temp;
		Vector temp2;
		Vector temp3;
		double b;
		temp = ray.origin;
		temp2 = p.normal;
		temp3 = multVectors(temp2,p.distance);
		temp2 = negative(temp3);
		addVectors(temp, temp2);
		b = dotProduct(p.normal,temp);
		return -1*b/a;
	}
}
*/

double findSphereIntersection(Ray ray, Sphere sphere){
	/*findSphere intersection, finds the intersection between
	the sphere and ray, returns root, -1 if missed*/
	double a = 1;
	double b = (2*(ray.origin.x - sphere.pos.x)*ray.direction.x) + (2*(ray.origin.y - sphere.pos.y)*ray.direction.y) + (2*(ray.origin.z - sphere.pos.z)*ray.direction.z);
	double c = pow(ray.origin.x - sphere.pos.x, 2) + pow(ray.origin.y - sphere.pos.y, 2) + pow(ray.origin.z - sphere.pos.z, 2) - (sphere.rad.totalRadius*sphere.rad.totalRadius);

	double discriminant = b*b - 4*c;

	if (discriminant > 0) {
		//ray intersects sphere
		//first root
		double root_1 = ((-1*b - sqrt(discriminant))/2) - 0.0001;
		if (root_1 > 0){
			//first root is smallest positive root
			return root_1;
		}
		else {
			//the second root is the smallest positive root			
			double root_2 = ((sqrt(discriminant) - b)/2) - 0.0001;
			return root_2;
		}
	}
	else {
		//ray missed sphere
		return -1;
	}
}

double testFindSphere (Ray ray, Sphere* sphere){
	double a = 1;
	double b = 2*(ray.direction.x*(ray.origin.x-sphere->pos.x)+ray.direction.y*(ray.origin.y-sphere->pos.y)+ray.direction.z*(ray.origin.z-sphere->pos.z));
	double c = pow(ray.origin.x - sphere->pos.x, 2) + pow(ray.origin.y - sphere->pos.y, 2) + pow(ray.origin.z - sphere->pos.z, 2) - pow(sphere->rad.totalRadius,2);
	
	double disc = pow(b,2)-4*c;

	double t0;
	double t1;

	t0 = (((-1)*b-sqrt(disc))/2);
	t1 = (((-1)*b+sqrt(disc))/2);

	if (disc < 0){
		//no intersection
		return -1;
	} else{
		if (t0>0){
			return t0;
		} else  if (t1>0){
			return t1;
		}
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
	//Ray declaration stuff
	int i,j;
	int x,y,z;
	double aspectratio;
	double xamount;
	double yamount;
	Camera camera;
	Vector X,Y,Z;
	Vector look_at;
	Vector diff_btw;
	Vector camdir;
	Vector camright;
	Vector camdown;
	Vector originVec;
	Sphere sphere;
	Plane plane;	
	Vector cam_ray_origin;
	Vector cam_ray_direction;
	Ray camera_ray;

	//temp vectors

	Vector innerTemp;
	Vector innerTemp2;
	Vector innerTemp3;

	Color white;
	Color green;
	Color black;
	Color planeColor;

	Vector light_pos;
	Vector campos;
	Light scene_light;


	//TESTING DELETE ME

	/*Vector vecTest1;
	Vector vecTest2;
	Vector test;

	vecTest1.x = -1;
	vecTest1.y = 1;
	vecTest1.z = -11;

	vecTest2.x = 4;
	vecTest2.y = 9;
	vecTest2.z = 3;

	printf("This is Vector val x %f",vecTest1.x);
	printf("This is Vector val y %f",vecTest1.y);
	printf("This is Vector val z %f",vecTest1.z);

	vecTest1 = normalize(vecTest1);

	printf("AFTER\n");

	printf("This is Vector val x %f",vecTest1.x);
	printf("This is Vector val y %f",vecTest1.y);
	printf("This is Vector val z %f",vecTest1.z);*/


	//END TEST

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

	planeColor.r = 0.5;
	planeColor.g = 0.25;
	planeColor.b = 0.25;

	//Sphere sphere is a sphere

	originVec.x = 0;
	originVec.y = 0;
	originVec.z = 0;

	//Scene light is white with vector or position
	scene_light.c = white;
	scene_light.v = light_pos;

	readFile();

	aspectratio = (double)screenWidth/(double)screenWidth;
	printf("%f",global.sph->pos.y);
	for (i = 0;i<screenWidth;i++){
		for (j = 0;j<screenWidth;j++){		
			double intersectionT;
						
			Vector p;
			Vector direction;
			Vector raySphereIntersection;

			p.x = i;
			p.y = j;			
			p.z = 0; //Need to determine proper z value
			
			//direction of ray
			//image is square
			/*xamount = (i+0.5)/screenWidth;
			yamount = ((screenWidth - j)+0.5)/screenWidth;
			cam_ray_origin = camera.campos;
			innerTemp3 = camright;
			multVectors(innerTemp3,(xamount-0.5));
			innerTemp = camdir;
			addVectors(innerTemp, innerTemp3);
			innerTemp2 = camdown;
			multVectors(innerTemp2,(yamount-0.5));
			innerTemp2 = normalize(innerTemp2);
			addVectors(innerTemp, innerTemp2);
			camera_ray.origin = cam_ray_origin;
			camera_ray.direction = innerTemp;*/

			innerTemp.x = 0;
			innerTemp.y = 0;
			innerTemp.z = 0;

			innerTemp2.x = 0;
			innerTemp2.y = 0;
			innerTemp2.z = 0;

			innerTemp3.x = 0;
			innerTemp3.y = 0;
			innerTemp3.z = 0;

			xamount = (i+0.5)/screenWidth;
			yamount = ((screenWidth-j)+0.5)/screenWidth;
			cam_ray_origin = camera.campos;
			innerTemp3 = camright;
			innerTemp3 = multVectors(innerTemp3,(xamount-0.5));
			innerTemp = camdir;
			innerTemp = addVectors(innerTemp, innerTemp3);
			innerTemp2 = camdown;
			innerTemp3 = multVectors(innerTemp2,(yamount-0.5));
			innerTemp2 = normalize(innerTemp2);
			innerTemp = addVectors(innerTemp, innerTemp2);
			camera_ray.origin = cam_ray_origin;
			//camera_ray.direction = innerTemp;

			direction.x = p.x - camera_ray.origin.x;
			direction.y = p.y - camera_ray.origin.y;
			direction.z = p.z - camera_ray.origin.z;

			direction = normalize(direction);		

			camera_ray.direction = direction;


			//loop through check each pixel for intersection
			/*printf("%fcamera origin x: ",camera_ray.origin.x);
			printf("\n");
			printf("%fcamera origin y: ",camera_ray.origin.y);
			printf("\n");
			printf("%fcamera origin z: ",camera_ray.origin.z);
			printf("\n");

			printf("%fcamera direction x: ",camera_ray.direction.x);
			printf("\n");
			printf("%fcamera direction y: ",camera_ray.direction.y);
			printf("\n");
			printf("%fcamera direction z: ",camera_ray.direction.z);
			printf("\n");*/

			//testingVar = testFindSphere(camera_ray,sphere);
			//printf("Sphere Intersection=%f\n", testFindSphere(camera_ray,sphere));
			intersectionT = testFindSphere(camera_ray,global.sph);
			if (intersectionT>0){
				//If the above finds a suitable positive t value, then it is used to nd the sphere intersection point ri
				printf("Sphere Intersection=%f\n", intersectionT);
				raySphereIntersection.x = camera_ray.origin.x + camera_ray.direction.x*intersectionT;
				raySphereIntersection.y = camera_ray.origin.y + camera_ray.direction.y*intersectionT;
				raySphereIntersection.z = camera_ray.origin.z + camera_ray.direction.z*intersectionT;

				printf("\n");
				printf("raySphereInterscetion.x %d",raySphereIntersection.x);
				printf("\n");
				printf("raySphereInterscetion.y %d",raySphereIntersection.y);
				printf("\n");
				printf("raySphereInterscetion.z %d",raySphereIntersection.z);
				printf("\n");
			}
			//printf("\n");

		}
	}

	//drawShapes();
    glutMainLoop();
	

}