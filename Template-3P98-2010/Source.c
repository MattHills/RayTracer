#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <freeglut.h>
#include <FreeImage.h>
#include <time.h>
#include <math.h>

#define MAX_RAY_DEPTH 3

//the pixel structure
typedef struct {
	GLubyte r, g, b;
} pixel;

typedef struct MaterialEffects{
	float trans;
	float Ra;
	float Rd;
	float Rs;
	float f;
} MaterialEffects;

// To angle on the shapes
typedef struct {
	float x;
	float y;
	float z;
} Angle;

// Between 0-255
typedef struct Colour{
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
	MaterialEffects eff;
	struct Donut *next;
} Donut;

// Plane shape
typedef struct Plane {
	int id;
	/*Position pos;	
	Size siz;
	Angle ang;	
	Transparency trans;*/
	Position normal;
	double distance;
	Colour col;
	struct Plane *next;
} Plane;

/*typedef struct Plane {
	int id;
	Vector normal;
	double distance;
	Colour col;
	struct Plane *next;
} Plane;*/

// Sphere shape
typedef struct Sphere {
	int id;
	Position pos;	
	Radius rad;
	Angle ang;
	Colour col;
	MaterialEffects eff;
	struct Sphere *next;
} Sphere;

// Rectangle shape
typedef struct Rect {
	int id;
	Position pos;	
	Size siz;
	Angle ang;
	Colour col;	
	MaterialEffects eff;
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
	MaterialEffects eff;
	struct Cylinder *next;
} Cylinder;

typedef struct Triangle {
	int id;
	Position pos;
	Size size;
	Angle ang;
	Colour col;	
	MaterialEffects eff;
	struct Triangle *next;
} Triangle;

typedef struct LightSource{
	int id;
	Position pos;
	Colour col;
	float Ia;
	float Is;
	struct LightSource *next;
} LightSource;

//the global structure
typedef struct {
	struct Cylinder *cyl;
	struct Rect *rec;
	struct Sphere *sph;
	struct Plane *pla;
	struct Donut *don;
	struct LightSource *lig;
	pixel *data;
	pixel *tempData;
} glob;
glob global;


//mag (sqrt((x*x) + (y*y) + (z*z))
//normalize = 
//invert negative -x,-y,-z
//dotproduct (vector v) return 
//crossproduct

typedef struct Ray{
	 Position origin;
	 Position direction;
} Ray;

typedef struct Camera{
	//Coord for scene for perspective
	Position campos;
	Position campdir;
	Position camright;
	Position camdown;
} Camera;

/*typedef struct Color {
	float r;
	float g;
	float b;
} Color;*/

typedef struct Light{
	struct Colour c;//color
	Position v;//position
} Light;

typedef struct Object{
	Colour color;
	double findIntersection;
} Object;


int screenWidth;	// size * cellSize

//write_img
void write_img(char *name, pixel *data, int width, int height) {
	FIBITMAP *image;
	RGBQUAD aPixel;
	int i,j;

	image = FreeImage_Allocate(width, height, 24, 0, 0, 0);
	if(!image) {
		perror("FreeImage_Allocate");
		return;
	}
	for(i = 0 ; i < height ; i++) {
		for(j = 0 ; j < width ; j++) {
			aPixel.rgbRed = data[i+j*width].r;
			aPixel.rgbGreen = data[i+j*width].g;
			aPixel.rgbBlue = data[i+j*width].b;

			FreeImage_SetPixelColor(image, j, i, &aPixel);
		}
	}
	if(!FreeImage_Save(FIF_TIFF, image, name, 0)) {
		perror("FreeImage_Save");
	}
	FreeImage_Unload(image);
	printf("Write Complete");
}//write_img

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
	LightSource *l;

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
				s->eff.trans = i;
				fscanf(file, "%f", &i);
				s->eff.Ra = i;
				fscanf(file, "%f", &i);
				s->eff.Rd = i;
				fscanf(file, "%f", &i);
				s->eff.Rs = i;
				fscanf(file, "%f", &i);
				s->eff.f = i;
				s->next = (Sphere*)malloc(sizeof (struct Sphere));
				s->next = 0;

				if(!global.sph){
					global.sph = (Sphere*)malloc(sizeof (struct Sphere));
					global.sph = s;
				}
				else{
					s->next = global.sph;
					global.sph = s;
				}
			}
			else if(i == 8){
				l = (LightSource*)malloc(sizeof (struct LightSource));
				fscanf(file, "%f", &i);
				l->pos.x = i;
				fscanf(file, "%f", &i);
				l->pos.y = i;
				fscanf(file, "%f", &i);
				l->pos.z = i;			
				fscanf(file, "%f", &i);
				l->col.r = i;	
				fscanf(file, "%f", &i);
				l->col.g = i;	
				fscanf(file, "%f", &i);
				l->col.b = i;	
				fscanf(file, "%f", &i);
				l->Ia = i;
				fscanf(file, "%f", &i);
				l->Is = i;
				l->next = (LightSource*)malloc(sizeof (struct LightSource));
				l->next = 0;

				if(!global.lig){
					global.lig = (LightSource*)malloc(sizeof (struct LightSource));
					global.lig = l;
				}
				else{
					l->next = global.lig;
					global.lig = l;
				}
			}
			fscanf(file, "%f", &i);
		}
		fclose(file);
	}	
}


//Position functions

double magnitude (Position v){
	return (sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z)));
}//returns magnitude of a Position

Position normalize (Position v){
	Position temp;
	temp.x = v.x/magnitude(v);
	temp.y = v.y/magnitude(v);
	temp.z = v.z/magnitude(v);
	return temp;
}//normalize a Position

Position negative (Position v){
	Position temp;
	temp.x = -v.x;
	temp.y = -v.y;
	temp.z = -v.z;
	return temp;
}//negative of a Position

float dotProduct(Position v, Position c){
	return (v.x*c.x + v.y*c.y + v.z*c.z);
}//dotProdcut of two Positions v and c

Position crossProd(Position v, Position c){
	Position temp;
	temp.x = (v.y*c.z-v.z*c.y);
	temp.y = (v.z*c.x-v.x*c.z);
	temp.z = (v.x*c.y-v.y*c.x);
	return temp;
}//returns the cross prod of two Positions


/*void addPositions(Position v, Position c){
	v.x = v.x+c.x;
	v.y = v.y+c.y;
	v.z = v.z+c.z;
}*/
//addtion of Position v and c
//sets Position v's coord to the new Position created
Position addPositions(Position v, Position c){
	Position temp;
	temp.x = v.x+c.x;
	temp.y = v.y+c.y;
	temp.z = v.z+c.z;
	return temp;
}


/*void multPositions(Position v, double scalar){
	v.x = v.x*scalar;
	v.y = v.y*scalar;
	v.z = v.z*scalar;
}*/
//multiple of Position v and scalar value
//sets Position v's coord to the new Position created

Position multPositions(Position v, double scalar){
	Position temp;
	temp.x = v.x*scalar;
	temp.y = v.y*scalar;
	temp.z = v.z*scalar;
	return temp;
}

//Plane intersection


double findIntersection(Ray ray,Plane* p){
	double a = dotProduct(ray.direction,p->normal);
	if (a == 0){//ray is parrallel to plane	
		return -1;
	}else{
		double b;
		Position temp;
		Position multTemp;
		temp = ray.origin;
		multTemp = multPositions(p->normal,p->distance);
		multTemp = negative(multTemp);
		temp = addPositions(p->normal, temp);
		b = dotProduct(p->normal, temp);
		return -1*b/a;
	}
}

double findSphereIntersection(Ray ray, Sphere* sphere){
	/*findSphere intersection, finds the intersection between
	the sphere and ray, returns root, -1 if missed*/
	double a = 1;
	double b = (2*(ray.origin.x - sphere->pos.x)*ray.direction.x) + (2*(ray.origin.y - sphere->pos.y)*ray.direction.y) + (2*(ray.origin.z - sphere->pos.z)*ray.direction.z);
	double c = pow(ray.origin.x - sphere->pos.x, 2) + pow(ray.origin.y - sphere->pos.y, 2) + pow(ray.origin.z - sphere->pos.z, 2) - (sphere->rad.totalRadius*sphere->rad.totalRadius);

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

//Planes

double findPlaneIntersection(Ray ray, Plane* plane){	
	//Calculate Plane normal Position dot Ray origin
	float a = dotProduct(plane->normal,ray.origin);
	//if a==0 ray is parrallel
	if (a == 0){
		return -1;
	} else {
		double b;
		double t;
		b = dotProduct(plane->normal, ray.direction);
		t = -1*a/b;	
		return t;
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

float calculateAmbient(float colour, float lightSourceColour, float Ia, float Ra){
	float ret;
	ret =  Ia * Ra;

	return colour * Ra;
}

float calculateDiffuse(float colour, Position lightToHitPoint, Position hitPointNormal, Position lightSourcePos, float lightSourceColour, float Is, float Rd){
	float a;
	float ret;

	// Get angle from 
	a = dotProduct(hitPointNormal, lightToHitPoint);
	if(a < 0){
		a = 0;
	}
	/*
	if(a > 0){
		ret = (colour + (lightSourceColour * Is * Rd * a)) / 2;
		if(ret > 255){
			return 255;
		}
		else{
			return ret;
		}
	}
	else{
		return 0;
	}
	*/
	return colour * (Is * Rd * a);
}

float calculateSpecular(float colour, Position lightRay, Position reflectionRay, float lightSourceColour, float Is, float Rs, float f){
	float ret;
	/*
	lightRay.x *= -1;
	lightRay.y *= -1;
	lightRay.z *= -1;
	*/
	ret = dotProduct(lightRay,reflectionRay);
	if(ret < 0){
		ret = 0;
	}
	ret = pow(ret,f);
	ret *= Is * Rs;

	return colour * ret;
}

Colour clipColour(Colour colour){
	float total = colour.r + colour.g + colour.b;
	float extra = total - 255 * 3;

	if(extra > 0){
		colour.r = colour.r*(colour.r/total);
		colour.g = colour.g*(colour.g/total);
		colour.b = colour.b*(colour.b/total);
	}
	if(colour.r > 255){
		colour.r = 255;
	}
	if(colour.b > 255){
		colour.b = 255;
	}
	if(colour.g > 255){
		colour.g = 255;
	}
	if(colour.r < 0){
		colour.r = 0;
	}
	if(colour.b < 0){
		colour.b = 255;
	}
	if(colour.g < 0){
		colour.g = 0;
	}
	
	return colour;
}


Position getSphereNormal(Position sphereCenter, Position hitPoint){
	Position v;	
	v.x = hitPoint.x - sphereCenter.x;
	v.y = hitPoint.y - sphereCenter.y;
	v.z = hitPoint.z - sphereCenter.z;	
	v = normalize(v);
	return v;
}

Position getReflectionRay(Position campos, Position hitPoint, Position normal){
	Position eyeRay;
	float dotProd;

	eyeRay.x = hitPoint.x - campos.x;
	eyeRay.y = hitPoint.y - campos.y;
	eyeRay.z = hitPoint.z - campos.z;
	eyeRay = normalize(eyeRay);

	dotProd = dotProduct(normal, eyeRay);
	eyeRay.x = eyeRay.x - 2 * dotProd * normal.x;
	eyeRay.y = eyeRay.y - 2 * dotProd * normal.y;
	eyeRay.z = eyeRay.z - 2 * dotProd * normal.z;

	return eyeRay;
}

void rayTrace(pixel* Im){
	//Ray declaration stuff
	int i,j;
	int x,y,z;
	double aspectratio;
	double xamount;
	double yamount;
	Camera camera;
	Position X,Y,Z;
	Position look_at;
	Position diff_btw;
	Position camdir;
	Position camright;
	Position camdown;
	Position originVec;
	Sphere sphere;
	Plane* plane;	
	Position cam_ray_origin;
	Position cam_ray_direction;
	Ray camera_ray;
	Sphere *testSphere;
	LightSource *lightSource;

	//temp Positions	

	Position innerTemp;
	Position innerTemp2;
	Position innerTemp3;

	Colour white;
	Colour green;
	Colour black;
	Colour planeColor;

	Position light_pos;
	Position campos;
	Light scene_light;

	//glClear(GL_COLOR_BUFFER_BIT);

	//glMatrixMode(GL_MODELVIEW);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glMatrixMode(GL_MODELVIEW);


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

	campos.x = screenWidth/2;
	campos.y = screenWidth/2;
	//campos.z = -10000;
	campos.z = -10;
	//campos.z = -1000;


	/*campos.x = 3;
	campos.y = 1.5;
	campos.z = -4;*/
	

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

	//Scene light is white with Position or position
	scene_light.c = white;
	scene_light.v = light_pos;

	readFile();

	aspectratio = (double)screenWidth/(double)screenWidth;

	//Plane stuff
	//Need to be part of file reader

	plane = (Plane*)malloc(sizeof (struct Plane));
	
	/*plane->normal.x = Y.x;
	plane->normal.y = Y.y;
	plane->normal.z = Y.z;*/

	plane->normal.x = X.x;
	plane->normal.y = X.y;
	plane->normal.z = X.z;

	plane->distance = -1;

	plane->col = green;

	//debugg
	
	/*printf("\n");
	printf("plane stuff normal x %f",plane->normal.x);
	printf("\n");
	printf("plane stuff normal y %f",plane->normal.y);
	printf("\n");
	printf("plane stuff normal z %f",plane->normal.z);*/
	

	//DEBUG STUFF
	/*global.sph->col = green;

	printf("\n");
	printf("sphere.x %f",global.sph->pos.x);
	printf("\n");
	printf("sphere.y %f",global.sph->pos.y);
	printf("\n");
	printf("sphere.z %f",global.sph->pos.z);
	printf("\n");
	printf("sphere.totalrad %f",global.sph->rad.totalRadius);
	printf("\n");
	printf("sphere.color.r %f",global.sph->col.r);
	printf("\n");
	printf("sphere.color.g %f",global.sph->col.g);
	printf("\n");
	printf("sphere.color.b %f",global.sph->col.b);
	printf("\n");*/

	for (i = 0;i<screenWidth;i++){
		for (j = 0;j<screenWidth;j++){		
			double intersectionT;
			double planeIntersection;
			float r,g,b;
			float r2,g2,b2;
						
			Position p;
			Position direction;
			Position raySphereIntersection;			
			Colour calcColour;

			p.x = i;
			p.y = j;			
			//p.z = -5000; //Need to determine proper z value
			p.z = 1000;
			
			//direction of ray
			//image is square
			/*xamount = (i+0.5)/screenWidth;
			yamount = ((screenWidth - j)+0.5)/screenWidth;
			cam_ray_origin = camera.campos;
			innerTemp3 = camright;
			multPositions(innerTemp3,(xamount-0.5));
			innerTemp = camdir;
			addPositions(innerTemp, innerTemp3);
			innerTemp2 = camdown;
			multPositions(innerTemp2,(yamount-0.5));
			innerTemp2 = normalize(innerTemp2);
			addPositions(innerTemp, innerTemp2);
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
			innerTemp3 = multPositions(innerTemp3,(xamount-0.5));
			innerTemp = camdir;
			innerTemp = addPositions(innerTemp, innerTemp3);
			innerTemp2 = camdown;
			innerTemp3 = multPositions(innerTemp2,(yamount-0.5));
			innerTemp2 = normalize(innerTemp2);
			innerTemp = addPositions(innerTemp, innerTemp2);
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
			//intersectionT = testFindSphere(camera_ray,global.sph);
			//intersectionT = testFindSphere(camera_ray,testSphere);

			
			planeIntersection = findIntersection(camera_ray, plane);

			if (planeIntersection>0){
				Im[i+j*screenWidth].r = 0;
				Im[i+j*screenWidth].b = 0;
				Im[i+j*screenWidth].g = 255;
			}
			testSphere = global.sph;
			while(testSphere){
				intersectionT = testFindSphere(camera_ray,testSphere);
				if (intersectionT>0){
					//If the above finds a suitable positive t value, then it is used to nd the sphere intersection point ri
					//printf("Sphere Intersection=%f\n", intersectionT);
					raySphereIntersection.x = camera_ray.origin.x + camera_ray.direction.x*intersectionT;
					raySphereIntersection.y = camera_ray.origin.y + camera_ray.direction.y*intersectionT;
					raySphereIntersection.z = camera_ray.origin.z + camera_ray.direction.z*intersectionT;

					/*
					printf("\n");
					printf("raySphereInterscetion.x %f",raySphereIntersection.x);
					printf("\n");
					printf("raySphereInterscetion.y %f",raySphereIntersection.y);
					printf("\n");
					printf("raySphereInterscetion.z %f",raySphereIntersection.z);
					printf("\n");
					*/

					//glColor3f(testSphere->col.r, testSphere->col.g, testSphere->col.b);
					//glColor3f(global.pla->col.r, global.pla->col.g, global.pla->col.b);
					//glBegin(GL_POINTS);
					//glVertex3f(raySphereIntersection.x,raySphereIntersection.y,raySphereIntersection.z);
					//glEnd();
					
					if(global.lig){
						r = 0;
						g = 0;
						b = 0;
						r2 = 0;
						g2 = 0;
						b2 = 0;
						lightSource = global.lig;
						while(lightSource){
							Position sphereNormal;
							Position lightVector;
							Position reflectionRay;

							// Get Vector normal from light source to hitpoint
							lightVector.x = lightSource->pos.x - raySphereIntersection.x;
							lightVector.y = lightSource->pos.y - raySphereIntersection.y;
							lightVector.z = lightSource->pos.z - raySphereIntersection.z;
							lightVector = normalize(lightVector);

							
							// Ambient Light Calculation
							r2 = calculateAmbient(testSphere->col.r, lightSource->col.r, lightSource->Ia, testSphere->eff.Ra);
							g2 = calculateAmbient(testSphere->col.g, lightSource->col.g, lightSource->Ia, testSphere->eff.Ra);
							b2 = calculateAmbient(testSphere->col.b, lightSource->col.b, lightSource->Ia, testSphere->eff.Ra);
						
							
							// Diffuse Reflection Calculation
							sphereNormal = getSphereNormal(testSphere->pos, raySphereIntersection);
							r2 += calculateDiffuse(testSphere->col.r, lightVector, sphereNormal, lightSource->pos, lightSource->col.r, lightSource->Is, testSphere->eff.Rd);
							g2 += calculateDiffuse(testSphere->col.g, lightVector, sphereNormal, lightSource->pos, lightSource->col.g, lightSource->Is, testSphere->eff.Rd);
							b2 += calculateDiffuse(testSphere->col.b, lightVector, sphereNormal, lightSource->pos, lightSource->col.b, lightSource->Is, testSphere->eff.Rd);
							
							
							// Specular Calculation
							
							reflectionRay = getReflectionRay(campos, raySphereIntersection, sphereNormal);
							r2 += calculateSpecular(testSphere->col.r, lightVector, reflectionRay, lightSource->col.r, lightSource->Is, testSphere->eff.Rs, testSphere->eff.f);
							g2 += calculateSpecular(testSphere->col.g, lightVector, reflectionRay, lightSource->col.g, lightSource->Is, testSphere->eff.Rs, testSphere->eff.f);
							b2 += calculateSpecular(testSphere->col.b, lightVector, reflectionRay, lightSource->col.b, lightSource->Is, testSphere->eff.Rs, testSphere->eff.f);
							

							r += r2;
							g += g2;
							b += b2;
							lightSource = lightSource->next;
						}						
						/*
						r *= testSphere->col.r;
						g *= testSphere->col.g;
						b *= testSphere->col.b;
						*/
					}
					else{
						r = testSphere->col.r;
						g = testSphere->col.g;
						b = testSphere->col.b;
					}

					calcColour.r = r;
					calcColour.g = g;
					calcColour.b = b;

					calcColour = clipColour(calcColour);

					Im[i+j*screenWidth].r = calcColour.r;					
					Im[i+j*screenWidth].g = calcColour.g;
					Im[i+j*screenWidth].b = calcColour.b;
				}
				testSphere = testSphere->next;
			}
		}
	}
	//glFlush();
}


main(int argc, char **argv)
{
	// Initialize game settings
	screenWidth = 800;	

	global.data = (pixel *)malloc((screenWidth)*(screenWidth)*sizeof(pixel *));

	global.tempData = (pixel *)malloc((screenWidth)*(screenWidth)*sizeof(pixel *));
	
	rayTrace(global.data);
	write_img("test.jpg", global.data, screenWidth, screenWidth);
	/*
	//printf("Q:quit\nU:toggle spray\nZ:spin on z\nX:spin on x\nY:spin on y\nC:toggle culling\nV:toggle colours\nD:toggle size\nI:view points\nO:view wire frame\nP:view polygons\nA:toggle auto fire particles\nS:set speed\nF:manual fire mode\nB:toggle rotate\nN:toggle friction\nR:reset\n");
    glutInit(&argc, argv);
    glutInitWindowSize(screenWidth, screenWidth);
	//glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitDisplayMode(GLUT_RGB);

    glutCreateWindow("Ray Tracer");

    glutKeyboardFunc(keyboard);
    glutDisplayFunc(rayTrace);
	//glutIdleFunc(redraw);
    glutReshapeFunc(reshape);

    glMatrixMode(GL_PROJECTION);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glLoadIdentity();

	glOrtho(0, screenWidth, 0, screenWidth, -100, 1000);//old working orth

	//glOrtho(0, screenWidth, 0, screenWidth, -1000, 100);

	//glOrtho(-10, 10, -10, 10, -1.0, 1.0);

	//gluOrtho2D(0,500,0,500);

	//glOrtho(-5,screenWidth,-5,screenWidth,0,10);
	//gluPerspective(20, screenWidth/screenWidth, 0.1, 1500);
	//gluLookAt(900, 250, 800, 0, 0, 0, 0, 1, 0);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	//glPushMatrix();
	//rayTrace();
    glutMainLoop();
	*/
	

}