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
	double trans;
	double If;
	double Ra;
	double Rd;
	double Rs;
	double f;
} MaterialEffects;

// Between 0-255
typedef struct Colour{
	double r;
	double g;
	double b;
} Colour;

// Position of shape
typedef struct {
	double x;
	double y;
	double z;
} Position;

// Radius for circular shapes
typedef struct {
	double internalRadius;
	double totalRadius;
} Radius;

// Torus shape
typedef struct Donut {
	int id;
	Position pos;
	Radius rad;
	Colour col;
	MaterialEffects eff;
	struct Donut *next;
} Donut;

typedef struct testObj{
	int id;
	Position pos;
	Position orient;
	Radius rad;
	double height;
	Colour col;
	MaterialEffects eff;
	struct testObj *next;
} testObj;

// Plane shape
typedef struct Plane {
	int id;
	Position pos;	
	Position norm;
	Colour col;
	int check;
	MaterialEffects eff;
	struct Plane *next;
} Plane;

// Sphere shape
typedef struct Sphere {
	int id;
	Position pos;	
	Radius rad;
	Colour col;
	MaterialEffects eff;
	struct Sphere *next;
} Sphere;

//infinite clinder
typedef struct Cylinder{
	int id;
	Position pos1;//bottom position of cylinder
	Position pos2;//top position of cylinder
	Position center;
	Radius rad;	
	double axis;
	Colour col;	
	MaterialEffects eff;
	struct Cylinder *next;
} Cylinder;

//Triangle
typedef struct Triangle {
	int id;
	Position A;
	Position B;
	Position C;
	Position normal;
	Colour col;
	MaterialEffects eff;
	struct Triangle *next;
} Triangle;

//Lightsource
typedef struct LightSource{
	int id;
	Position pos;
	Colour col;
	double Ia;
	double Is;
	struct LightSource *next;
} LightSource;

//the global structure
typedef struct {
	struct Cylinder *cyl;
	struct Rect *rec;
	struct Sphere *sph;
	struct Plane *pla;
	struct Donut *don;
	struct Triangle *tri;
	struct LightSource *lig;
	pixel *data;
	pixel *tempData;
	int idCount;
	int currentId;
	double prevRefra;
	double prevRefraId;
	double recursionCount;
	int numLS;
} glob;
glob global;


//Ray Vector
typedef struct Ray{
	 Position origin;
	 Position direction;
} Ray;

//Camera struct, for o
typedef struct Camera{
	//Coord for scene for perspective
	Position campos;
	Position campdir;
	Position camright;
	Position camdown;
} Camera;

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
//writes to test.jpg image
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

			FreeImage_SetPixelColor(image, i, j, &aPixel);
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
   }//Quit
}

//Read file, reads in ray details stores objects in a list
void readFile(){
	FILE *file;
	double i;
	Sphere *s;
	Plane *p;
	Triangle *t;
	LightSource *l;

	file = fopen("raydetails.txt","r"); 

	if(!file){
		printf("Error when opening file");
	}
	else{
		fscanf(file, "%lf", &i);
		while(!feof(file)){
			if(i == 0){ //loading spheres
				s = (Sphere*)malloc(sizeof (struct Sphere));
				s->id = global.idCount;
				global.idCount++;
				fscanf(file, "%lf", &i);
				s->pos.x = i;
				fscanf(file, "%lf", &i);
				s->pos.y = i;
				fscanf(file, "%lf", &i);
				s->pos.z = i;
				fscanf(file, "%lf", &i);
				s->rad.totalRadius = i;
				fscanf(file, "%lf", &i);
				s->col.r = i;
				fscanf(file, "%lf", &i);
				s->col.g = i;
				fscanf(file, "%lf", &i);
				s->col.b = i;
				fscanf(file, "%lf", &i);
				s->eff.trans = i;
				fscanf(file, "%lf", &i);
				s->eff.If = i;
				fscanf(file, "%lf", &i);
				s->eff.Ra = i;
				fscanf(file, "%lf", &i);
				s->eff.Rd = i;
				fscanf(file, "%lf", &i);
				s->eff.Rs = i;
				fscanf(file, "%lf", &i);
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
			else if(i == 1){ //loading planes
				p = (Plane*)malloc(sizeof (struct Plane));
				p->id = global.idCount;
				global.idCount++;
				fscanf(file, "%lf", &i);
				p->norm.x = i;
				fscanf(file, "%lf", &i);
				p->norm.y = i;
				fscanf(file, "%lf", &i);
				p->norm.z = i;
				fscanf(file, "%lf", &i);
				p->pos.x = i;
				fscanf(file, "%lf", &i);
				p->pos.y = i;
				fscanf(file, "%lf", &i);
				p->pos.z = i;
				fscanf(file, "%lf", &i);
				p->col.r = i;
				fscanf(file, "%lf", &i);
				p->col.g = i;
				fscanf(file, "%lf", &i);
				p->col.b = i;
				fscanf(file, "%lf", &i);
				p->check = i;
				fscanf(file, "%lf", &i);
				p->eff.trans = i;
				fscanf(file, "%lf", &i);
				p->eff.If = i;
				fscanf(file, "%lf", &i);
				p->eff.Ra = i;
				fscanf(file, "%lf", &i);
				p->eff.Rd = i;
				fscanf(file, "%lf", &i);
				p->eff.Rs = i;
				fscanf(file, "%lf", &i);
				p->eff.f = i;
				p->next = (Plane*)malloc(sizeof (struct Plane));
				p->next = 0;

				if(!global.pla){
					global.pla = (Plane*)malloc(sizeof (struct Plane));
					global.pla = p;
				}
				else{
					p->next = global.pla;
					global.pla = p;
				}
			}
			else if(i == 2){ //loading triangles
				t = (Triangle*)malloc(sizeof (struct Triangle));
				t->id = global.idCount;
				global.idCount++;
				fscanf(file, "%lf", &i);
				t->A.x = i;
				fscanf(file, "%lf", &i);
				t->A.y = i;
				fscanf(file, "%lf", &i);
				t->A.z = i;
				fscanf(file, "%lf", &i);
				t->B.x = i;
				fscanf(file, "%lf", &i);
				t->B.y = i;
				fscanf(file, "%lf", &i);
				t->B.z = i;
				fscanf(file, "%lf", &i);
				t->C.x = i;
				fscanf(file, "%lf", &i);
				t->C.y = i;
				fscanf(file, "%lf", &i);
				t->C.z = i;
				fscanf(file, "%lf", &i);
				t->col.r = i;
				fscanf(file, "%lf", &i);
				t->col.g = i;
				fscanf(file, "%lf", &i);
				t->col.b = i;
				fscanf(file, "%lf", &i);
				t->eff.trans = i;
				fscanf(file, "%lf", &i);
				t->eff.If = i;
				fscanf(file, "%lf", &i);
				t->eff.Ra = i;
				fscanf(file, "%lf", &i);
				t->eff.Rd = i;
				fscanf(file, "%lf", &i);
				t->eff.Rs = i;
				fscanf(file, "%lf", &i);
				t->eff.f = i;
				t->next = (Triangle*)malloc(sizeof (struct Triangle));
				t->next = 0;

				if(!global.tri){
					global.tri = (Triangle*)malloc(sizeof (struct Triangle));
					global.tri = t;
				}
				else{
					t->next = global.tri;
					global.tri = t;
				}
			}
			else if(i == 8){ //loading lightsource
				l = (LightSource*)malloc(sizeof (struct LightSource));
				l->id = global.idCount;
				global.idCount++;
				fscanf(file, "%lf", &i);
				l->pos.x = i;
				fscanf(file, "%lf", &i);
				l->pos.y = i;
				fscanf(file, "%lf", &i);
				l->pos.z = i;			
				fscanf(file, "%lf", &i);
				l->col.r = i;	
				fscanf(file, "%lf", &i);
				l->col.g = i;	
				fscanf(file, "%lf", &i);
				l->col.b = i;	
				fscanf(file, "%lf", &i);
				l->Ia = i;
				fscanf(file, "%lf", &i);
				l->Is = i;
				l->next = (LightSource*)malloc(sizeof (struct LightSource));
				l->next = 0;

				if(!global.lig){
					global.lig = (LightSource*)malloc(sizeof (struct LightSource));
					global.lig = l;
					global.numLS = 1;
				}
				else{
					l->next = global.lig;
					global.lig = l;
					global.numLS++;
				}
			}
			fscanf(file, "%lf", &i);
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

double dotProduct(Position v, Position c){
	return (v.x*c.x + v.y*c.y + v.z*c.z);
}//dotProdcut of two Positions v and c

Position crossProd(Position v, Position c){
	Position temp;
	temp.x = (v.y*c.z-v.z*c.y);
	temp.y = (v.z*c.x-v.x*c.z);
	temp.z = (v.x*c.y-v.y*c.x);
	return temp;
}//returns the cross prod of two Positions

//addtion of Position v and c
//sets Position v's coord to the new Position created
Position addPositions(Position v, Position c){
	Position temp;
	temp.x = v.x+c.x;
	temp.y = v.y+c.y;
	temp.z = v.z+c.z;
	return temp;
}

//sets Position v's coord to the new Position created

//Multiply Positions, returns new position 
Position multPositions(Position v, double scalar){
	Position temp;
	temp.x = v.x*scalar;
	temp.y = v.y*scalar;
	temp.z = v.z*scalar;
	return temp;
}

//findSphereIntersection
//Finds the intersection of a ray and a sphere returns t0, or t1 if disc positive
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
	return -1;
}

//Plane Intersection 
//Calculates ray and plane intersection, returns t0, t1
double findPlaneIntersection(Ray ray, Plane* plane){
	Position planeNormal;
	Position rayOrigin;
	double a;
	//Calculate Plane normal Position dot Ray origin
	planeNormal = normalize(plane->norm);
	rayOrigin = normalize(ray.origin);
	a = dotProduct(plane->pos,ray.origin);
	//if a==0 ray is parrallel
	if (a == 0){
		return -1;
	} else {
		double b;
		double t;
		b = dotProduct(planeNormal, ray.direction);
		t = -1*a/b;	
		return t;
	}
	return -1;
}

//Triangles
//Returns the normal position vector of a triangle based on ABC positions
Position getTriangleNormal(Position A, Position B, Position C){
	Position CA;
	Position BA;
	Position normal;
	CA.x = C.x - A.x;
	CA.y = C.y - A.y;
	CA.z = C.z - A.z;
	BA.x = B.x - A.x;
	BA.y = B.y - A.y;
	BA.z = B.z - A.z;
	normal = crossProd(CA, BA);
	normal = normalize(normal);
	return normal;
}//return normal of triangle


//Returns the distance value of the dotproduct
//of the triangle normal and point A
double getTriangleDistance(Triangle* t){
	double distance;
	distance = dotProduct(t->normal, t->A);
	return distance;
}

Position getNormalAt(Triangle t){
	return t.normal;
}

// From
// http://www.blackpawn.com/texts/pointinpoly/
double findTriIntersect(Ray ray,Triangle* t){
	Position v0,v1,v2,A,B,C, rayNorm;
	double dot00,dot01,dot02,dot11,dot12;
	double invDenom;
	double u,v;	
	
	// Normalizing
	A = normalize(t->A);
	B = normalize(t->B);
	C = normalize(t->C);
	rayNorm = normalize(ray.direction);

	//C - A
	v0.x = C.x - A.x;
	v0.y = C.y - A.y;
	v0.z = C.z - A.z;

	//B-A
	v1.x = B.x - A.x;
	v1.y = B.y - A.y;
	v1.z = B.z - A.z;

	//P-A
	v2.x = rayNorm.x - A.x;
	v2.y = rayNorm.y - A.y;
	v2.z = rayNorm.z - A.z;

	dot00 = dotProduct(v0,v0);
	dot01 = dotProduct(v0, v1);
	dot02 = dotProduct(v0, v2);
	dot11 = dotProduct(v1, v1);
	dot12 = dotProduct(v1, v2);

	invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	v = (dot00 * dot12 - dot01 * dot02) * invDenom;
	if ((u >= 0) && (v >= 0) && (u + v < 1)){
		return 1;
	} else {
		return -1;
	}
}

//Calculates ambient lighting on object
double calculateAmbient(double colour, double lightSourceColour, double Ia, double Ra){
	double ret;
	ret =  Ia * Ra;
	return colour * Ra;
}

//Calculates diffuse
double calculateDiffuse(double colour, Position lightToHitPoint, Position hitPointNormal, Position lightSourcePos, double lightSourceColour, double Is, double Rd){
	double a;

	// Get angle from 
	a = dotProduct(hitPointNormal, lightToHitPoint);
	if(a < 0){
		a = 0;
	}
	return colour * (Is * Rd * a);
}

//Calculates specular lighting
double calculateSpecular(double colour, Position lightRay, Position reflectionRay, double lightSourceColour, double Is, double Rs, double f){
	double ret;

	ret = dotProduct(lightRay,reflectionRay);
	if(ret < 0){
		ret = 0;
	}
	ret = pow(ret,f);
	ret *= Is * Rs;

	return lightSourceColour * ret;
}

//ClipColour ensures colors are within color spectrum
Colour clipColour(Colour colour){
	colour.r = colour.r / global.numLS;
	colour.g = colour.g /global.numLS;
	colour.b = colour.b /global.numLS;
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

//Returns calculated sphere's normal at hit point
Position getSphereNormal(Sphere *sphere, Position hitPoint){
	Position v;	
	v.x = (hitPoint.x - sphere->pos.x) / sphere->rad.totalRadius;
	v.y = (hitPoint.y - sphere->pos.y) / sphere->rad.totalRadius;
	v.z = (hitPoint.z - sphere->pos.z) / sphere->rad.totalRadius;	
	v = normalize(v);
	return v;
}

//Returns calculated reflection ray
Position getReflectionRay(Position campos, Position hitPoint, Position normal){
	Position eyeRay;
	double dotProd;

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

Position calcRefraction(Position campos, Position hitPoint, Position normal, double iF){
	Position eyeRay;
	Position ret;
	double dotProd, temp;

	eyeRay.x = hitPoint.x - campos.x;
	eyeRay.y = hitPoint.y - campos.y;
	eyeRay.z = hitPoint.z - campos.z;
	eyeRay = normalize(eyeRay);

	dotProd = -dotProduct(normal, eyeRay);
	eyeRay.x = eyeRay.x + 2 * dotProd * normal.x;
	eyeRay.y = eyeRay.y + 2 * dotProd * normal.y;
	eyeRay.z = eyeRay.z + 2 * dotProd * normal.z;

	temp = sqrt(1 - pow(iF, 2) * (1 - pow(dotProd, 2)));

	if(temp > 0){
		ret.x = (iF * eyeRay.x) + (iF * dotProd - temp) * normal.x;
		ret.y = (iF * eyeRay.y) + (iF * dotProd - temp) * normal.y;
		ret.z = (iF * eyeRay.z) + (iF * dotProd - temp) * normal.z;
	}
	else{
		ret.x = -99999999999;
		ret.y = -99999999999;
		ret.z = -99999999999;
	}

	return ret;
}

//Determines the closests intersection point of object and ray
//finds closest object to camera
int findClosestIntersectionPoint(Ray cameraRay){
	double currentClosestZ;
	int returnId;
	double intersectionT;
	double intersectionZVal;
	double tolerance = 0.00001;
	Sphere *testSphere;
	Plane *testPlane;
	Triangle *testTriangle;

	testSphere = global.sph;
	testPlane = global.pla;
	testTriangle = global.tri;

	returnId = -1;
	currentClosestZ = 10000000;

	while(testSphere && testSphere->id != global.currentId){
		intersectionT = testFindSphere(cameraRay, testSphere);		
		if(intersectionT > 0){
			if(abs(intersectionT) > tolerance){
				intersectionZVal = cameraRay.origin.z + cameraRay.direction.z*intersectionT;
				if(intersectionZVal < currentClosestZ){
					currentClosestZ = intersectionZVal;
					returnId = testSphere->id;
				}
			}
		}
		testSphere = testSphere->next;
	}
	while(testPlane && testPlane->id != global.currentId){
		intersectionT = findPlaneIntersection(cameraRay, testPlane);
		if(intersectionT > 0){
			if(abs(intersectionT) > tolerance){
				intersectionZVal = cameraRay.origin.z + cameraRay.direction.z*intersectionT;
				if(intersectionZVal < currentClosestZ){
					currentClosestZ = intersectionZVal;
					returnId = testPlane->id;
				}
			}
		}
		testPlane = testPlane->next;
	}

	while(testTriangle && testTriangle->id != global.currentId){
		intersectionT = findTriIntersect(cameraRay, testTriangle);
		if(intersectionT > 0){
			if(abs(intersectionT) > tolerance){
				intersectionZVal = cameraRay.origin.z + cameraRay.direction.z*intersectionT;
				if(intersectionZVal < currentClosestZ){
					currentClosestZ = intersectionZVal;
					returnId = testTriangle->id;
				}
			}
		}
		testTriangle = testTriangle->next;
	}

	return returnId;
}

double square(double x){
	return x * x;
}

int findClearPath(Ray hitPointVector, LightSource *lightSource, int closestId){
	Sphere *testSphere;
	Plane *testPlane;
	Triangle *testTriangle;
	double intersectionT;
	Ray normalized;

	testSphere = global.sph;
	testPlane = global.pla;
	testTriangle = global.tri;
	normalized.direction = normalize(hitPointVector.direction);
	normalized.origin = hitPointVector.origin;
	// Perhaps reverse the orgin/hitpoint if the light is in a different direction
	while(testSphere){
		if(testSphere->id != closestId){
			intersectionT = testFindSphere(normalized, testSphere);
			if(intersectionT > 0){
				return 0;
			}
		}
		testSphere = testSphere->next;
	}
	
	while(testPlane){
		if(testPlane->id != closestId){
			intersectionT = findPlaneIntersection(hitPointVector, testPlane);
			if(intersectionT > 0){
				return 0;
			}
		}
		testPlane = testPlane->next;
	}
	
	while(testTriangle){
		intersectionT = findTriIntersect(hitPointVector, testTriangle);
		if(intersectionT > 0){
			return 0;
		}
		testTriangle = testTriangle->next;
	}
	
	return 1;
}


Colour rayTrace(Ray ray){
	Sphere *testSphere;
	Plane *testPlane;
	Triangle *testTriangle;
	LightSource *lightSource;
	Colour calcColour, origColour;
	int closestId;
	double intersectionT;
	Position rayIntersection, calcNormal;
	double r,g,b;
	double r2,g2,b2;				
	double Ra=0, Rd=0, Rs=0, f=0;
	double transparency = 0, If=0;
	int hitPoint=0;
	Ray reflectionRay;


	testSphere = global.sph;
	testPlane = global.pla;
	testTriangle = global.tri;

	closestId = findClosestIntersectionPoint(ray);

	while(testSphere){
		if(testSphere->id == closestId){
			global.currentId = closestId;
			break;
		}
		testSphere = testSphere->next;
	}

	while(testPlane){
		if(testPlane->id == closestId){
			global.currentId = closestId;
			break;
		}
		testPlane = testPlane->next;
	}

	while(testTriangle){
		if(testTriangle->id == closestId){
			global.currentId = closestId;
			break;
		}
		testTriangle = testTriangle->next;
	}

	if(testSphere){
		intersectionT = testFindSphere(ray, testSphere);
		rayIntersection.x = ray.origin.x + ray.direction.x*intersectionT;
		rayIntersection.y = ray.origin.y + ray.direction.y*intersectionT;
		rayIntersection.z = ray.origin.z + ray.direction.z*intersectionT;
				
		calcNormal = getSphereNormal(testSphere, rayIntersection);				

		transparency = testSphere->eff.trans;
		If = testSphere->eff.If;
		Ra = testSphere->eff.Ra;
		Rd = testSphere->eff.Rd;
		Rs = testSphere->eff.Rs;
		f = testSphere->eff.f;

		origColour.r = testSphere->col.r;
		origColour.g = testSphere->col.g;
		origColour.b = testSphere->col.b;

		calcColour.r = testSphere->col.r;
		calcColour.g = testSphere->col.g;
		calcColour.b = testSphere->col.b;

		hitPoint = 1;
	}
	else if(testPlane){	
		int square;

		intersectionT = findPlaneIntersection(ray, testPlane);
		rayIntersection.x = ray.origin.x + ray.direction.x*intersectionT;
		rayIntersection.y = ray.origin.y + ray.direction.y*intersectionT;
		rayIntersection.z = ray.origin.z + ray.direction.z*intersectionT;

		calcNormal = normalize(testPlane->norm);

		transparency = testPlane->eff.trans;
		If = testPlane->eff.If;
		Ra = testPlane->eff.Ra;
		Rd = testPlane->eff.Rd;
		Rs = testPlane->eff.Rs;
		f = testPlane->eff.f;

		if(testPlane->check == 99){
			square = (int)(rayIntersection.x/40) + (int)(rayIntersection.y/40) + (int)(rayIntersection.z/40);

			if((square % 2) ==0){
				origColour.r = 0;
				origColour.g = 0;
				origColour.b = 0;

				calcColour.r = 0;
				calcColour.g = 0;
				calcColour.b = 0;
			}
			else{
				origColour.r = 255;
				origColour.g = 255;
				origColour.b = 255;

				calcColour.r = 255;
				calcColour.g = 255;
				calcColour.b = 255;
			}
		}
		else{		
			origColour.r = testPlane->col.r;
			origColour.g = testPlane->col.g;
			origColour.b = testPlane->col.b;

			calcColour.r = testPlane->col.r;
			calcColour.g = testPlane->col.g;
			calcColour.b = testPlane->col.b;		
		}

		hitPoint = 1;
	}
	else if(testTriangle){
		intersectionT = findTriIntersect(ray, testTriangle);				
		rayIntersection.x = ray.origin.x + ray.direction.x*intersectionT;
		rayIntersection.y = ray.origin.y + ray.direction.y*intersectionT;
		rayIntersection.z = ray.origin.z + ray.direction.z*intersectionT;

		calcNormal = getTriangleNormal(testTriangle->A,testTriangle->B,testTriangle->C);

		transparency = testTriangle->eff.trans;
		If = testTriangle->eff.If;
		Ra = testTriangle->eff.Ra;
		Rd = testTriangle->eff.Rd;
		Rs = testTriangle->eff.Rs;
		f = testTriangle->eff.f;

		origColour.r = testTriangle->col.r;
		origColour.g = testTriangle->col.g;
		origColour.b = testTriangle->col.b;

		calcColour.r = testTriangle->col.r;
		calcColour.g = testTriangle->col.g;
		calcColour.b = testTriangle->col.b;

		hitPoint = 1;
	}
	else{
		transparency = 0;
		Ra = 0.5;
		Rd = 0.5;
		Rs = 0.5;
		f = 1;

		origColour.r = 0;
		origColour.g = 0;
		origColour.b = 0;

		calcColour.r = 0;
		calcColour.g = 0;
		calcColour.b = 0;

		hitPoint = 0;
	}
	if(global.lig && hitPoint == 1){
		int clearPath = 1;
		r = 0;
		g = 0;
		b = 0;
		r2 = 0;
		g2 = 0;
		b2 = 0;
		lightSource = global.lig;
		while(lightSource){
			Position lightVector;
			Ray hitRay;
			Colour traceColour;

			// Get Vector normal from light source to hitpoint
			lightVector.x = lightSource->pos.x - rayIntersection.x;
			lightVector.y = lightSource->pos.y - rayIntersection.y;
			lightVector.z = lightSource->pos.z - rayIntersection.z;
			lightVector = normalize(lightVector);

			// Ambient Light Calculation
			r2 = calculateAmbient(origColour.r, lightSource->col.r, lightSource->Ia, Ra);
			g2 = calculateAmbient(origColour.g, lightSource->col.g, lightSource->Ia, Ra);
			b2 = calculateAmbient(origColour.b, lightSource->col.b, lightSource->Ia, Ra);

			// Find if point is in shadow
			hitRay.origin = rayIntersection;
			hitRay.direction = lightSource->pos;

			clearPath = findClearPath(hitRay, lightSource, closestId);

			// Reflection Calculation
			reflectionRay.origin = rayIntersection;
			reflectionRay.direction = getReflectionRay(ray.origin, rayIntersection, calcNormal);

			if(clearPath == 1){	
				// Diffuse Reflection Calculation
				r2 += calculateDiffuse(origColour.r, lightVector, calcNormal, lightSource->pos, lightSource->col.r, lightSource->Is, Rd);
				g2 += calculateDiffuse(origColour.g, lightVector, calcNormal, lightSource->pos, lightSource->col.g, lightSource->Is, Rd);
				b2 += calculateDiffuse(origColour.b, lightVector, calcNormal, lightSource->pos, lightSource->col.b, lightSource->Is, Rd);
					
				// Specular Calculation
				r2 += calculateSpecular(origColour.r, lightVector, reflectionRay.direction, lightSource->col.r, lightSource->Is, Rs, f);
				g2 += calculateSpecular(origColour.g, lightVector, reflectionRay.direction, lightSource->col.g, lightSource->Is, Rs, f);
				b2 += calculateSpecular(origColour.b, lightVector, reflectionRay.direction, lightSource->col.b, lightSource->Is, Rs, f);				
			}

			if(Rs > 0.5 && global.recursionCount < 5){
				global.recursionCount++;
				traceColour = rayTrace(reflectionRay);
				global.recursionCount--;
				r2 += traceColour.r;
				g2 += traceColour.g;
				b2 += traceColour.b;
			}
				
			// Refraction Calculation
			if(transparency == 1 && global.recursionCount < 5){
				double n;
				double tempPrevRefra;
				double tempPrevId;
				Ray refractedRay;

				n = global.prevRefra /  If;
				refractedRay.direction = calcRefraction(ray.origin, rayIntersection, calcNormal, n);

				if(refractedRay.direction.x != -99999999999 && refractedRay.direction.y != -99999999999
					&& refractedRay.direction.z != -99999999999){
					refractedRay.origin = rayIntersection;
					//refractedRay.direction = negative(refractedRay.direction);
					tempPrevId = global.prevRefra;
					tempPrevRefra = global.prevRefra;
					global.prevRefra = If;
					global.recursionCount++;

					traceColour = rayTrace(refractedRay);			
					global.prevRefra = tempPrevRefra;
					global.recursionCount--;

					if(traceColour.r == 0 && traceColour.g == 0 && traceColour.b == 0){
						r2 *= 0.4;
						g2 *= 0.4;
						b2 *= 0.4;
					}
					else{
						r2 += traceColour.r;
						g2 += traceColour.g;
						b2 += traceColour.b;
					}					
					
				}				
			}
			r += r2;
			g += g2;
			b += b2;
			lightSource = lightSource->next;
		}
		calcColour.r = r;
		calcColour.g = g;
		calcColour.b = b;
		calcColour = clipColour(calcColour);
				
	}

	return calcColour;
}

void startTrace(pixel* Im){
	//Ray declaration stuff
	int i,j,k,l;
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
	Ray camera_ray;
	testObj *testObject;
	double fovx, fovy;
	int aadepth = 1;
	Colour aaColour;

	//temp Positions	

	Position innerTemp;
	Position innerTemp2;
	Position innerTemp3;
	Position campos;

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

	fovx = 3.14159265358979323846 / 4;
	fovy = screenWidth / screenWidth * fovx;

	campos.x = 400;
	campos.y = 100;
	campos.z = -800;

	look_at.x = 400;
	look_at.y = 100;
	look_at.z = 0;

	diff_btw.x = campos.x-look_at.x;
	diff_btw.y = campos.y-look_at.y;
	diff_btw.z = campos.z-look_at.z;
	
	camdir = normalize(negative(diff_btw));
	camright = normalize(crossProd(camdir,Y));
	camdown = crossProd(camdir,camright);

	camera.campos = campos;
	camera.camdown = camdir;
	camera.camright = camright;
	camera.campdir = camdir;

	readFile();

	aspectratio = (double)screenWidth/(double)screenWidth;

	for (i = 0;i<screenWidth;i++){
		for (j = 0;j<screenWidth;j++){
			Colour calcColour;
			aaColour.r = 0;
			aaColour.g = 0;
			aaColour.b = 0;
			//Uncomment for Anti-aliasing
			//for(k = 0; k < 4; k++){
				//for(l = 0; l < 4; l++){
					
			
					innerTemp.x = 0;
					innerTemp.y = 0;
					innerTemp.z = 0;

					innerTemp2.x = 0;
					innerTemp2.y = 0;
					innerTemp2.z = 0;

					innerTemp3.x = 0;
					innerTemp3.y = 0;
					innerTemp3.z = 0;


					//Comment out for AA
					xamount = (screenWidth - i +0.5)/screenWidth;
					yamount = ((screenWidth-j)+0.5)/screenWidth;
					//Uncomment for AA
					//xamount = (screenWidth - i +0.5 *k/4)/screenWidth;
					//yamount = ((screenWidth-j)+0.5 *l/4)/screenWidth;
					innerTemp = multPositions(camdown, yamount - 0.5);
					innerTemp2 = multPositions(camright, xamount - 0.5);
					innerTemp2 = addPositions(innerTemp2, innerTemp);
					innerTemp2 = addPositions(camdir, innerTemp2);
					innerTemp2 = normalize(innerTemp2);
			
					camera_ray.origin = camera.campos;
					camera_ray.direction = innerTemp2;

					global.currentId = -1;
					global.recursionCount = 0;

					calcColour = rayTrace(camera_ray);	
					aaColour.r += calcColour.r;
					aaColour.g += calcColour.g;
					aaColour.b += calcColour.b;
			
					//Uncomment for AA
				//}
			//}
			//Comment for AA
			Im[i+j*screenWidth].r = aaColour.r;					
			Im[i+j*screenWidth].g = aaColour.g;
			Im[i+j*screenWidth].b = aaColour.b;
			//Uncomment for AA
			//Im[i+j*screenWidth].r = aaColour.r/16;					
			//Im[i+j*screenWidth].g = aaColour.g/16;
			//Im[i+j*screenWidth].b = aaColour.b/16;
		}
	}
}


main(int argc, char **argv)
{
	// Initialize game settings
	screenWidth = 800;	

	global.idCount = 0;

	global.data = (pixel *)malloc((screenWidth)*(screenWidth)*sizeof(pixel *));

	global.tempData = (pixel *)malloc((screenWidth)*(screenWidth)*sizeof(pixel *));
	
	startTrace(global.data);
	write_img("test.jpg", global.data, screenWidth, screenWidth);	

}