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
	double Ra;
	double Rd;
	double Rs;
	double f;
} MaterialEffects;

// To angle on the shapes
typedef struct {
	double x;
	double y;
	double z;
} Angle;

// Between 0-255
typedef struct Colour{
	double r;
	double g;
	double b;
} Colour;

// Size of shape
typedef struct {
	double height;
	double width;
	double depth;
} Size;

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
	Angle ang;
	Colour col;
	MaterialEffects eff;
	struct Donut *next;
} Donut;

typedef struct testObj{
	int id;
	Position pos1;
	Position pos2;
	Radius rad;
	Colour col;
	MaterialEffects eff;
	struct testObj *next;
} testObj;

// Plane shape
typedef struct Plane {
	int id;
	/*Position pos;	
	Size siz;
	Angle ang;	
	Transparency trans;*/
	Position normal;
	Colour col;
	MaterialEffects eff;
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
/*typedef struct Cylinder{
	int id;
	Position pos;	
	Size size;
	Radius rad;	
	Angle ang;
	Colour col;	
	MaterialEffects eff;
	struct Cylinder *next;
} Cylinder;*/

//infinite clinder
typedef struct Cylinder{
	int id;
	Position pos1;//bottom position of cylinder
	Position pos2;//top position of cylinder
	Position center;
	Radius rad;	
	double axis;
	//double distance;
	Colour col;	
	MaterialEffects eff;
	struct Cylinder *next;
} Cylinder;

typedef struct Triangle {
	int id;
	Position A;
	Position B;
	Position C;
	Position normal;
	double distance;
	Colour col;
	MaterialEffects eff;
	struct Triangle *next;
} Triangle;

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
	double r;
	double g;
	double b;
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
	double i;
	Sphere *s;
	Plane *p;
	Triangle *t;
	LightSource *l;

	//file = fopen("C:\3P98\3P98 Final Project\Ray Tracer\RayTracer\raydetails.txt","r");
	file = fopen("raydetails.txt","r");

	if(!file){
		printf("Error when opening file");
	}
	else{
		fscanf(file, "%lf", &i);
		while(!feof(file)){
			if(i == 0){
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
				s->ang.x = 0;
				s->ang.y = 0;
				s->ang.z = 0;
				fscanf(file, "%lf", &i);
				s->col.r = i;
				fscanf(file, "%lf", &i);
				s->col.b = i;
				fscanf(file, "%lf", &i);
				s->col.g = i;
				fscanf(file, "%lf", &i);
				s->eff.trans = i;
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
			else if(i == 1){
				p = (Plane*)malloc(sizeof (struct Plane));
				p->id = global.idCount;
				global.idCount++;
				fscanf(file, "%lf", &i);
				p->normal.x = i;
				fscanf(file, "%lf", &i);
				p->normal.y = i;
				fscanf(file, "%lf", &i);
				p->normal.z = i;
				fscanf(file, "%lf", &i);
				p->col.r = i;
				fscanf(file, "%lf", &i);
				p->col.g = i;
				fscanf(file, "%lf", &i);
				p->col.b = i;
				fscanf(file, "%lf", &i);
				p->eff.trans = i;
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
			else if(i == 2){
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
			else if(i == 8){
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
				}
				else{
					l->next = global.lig;
					global.lig = l;
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

double findIntersectionTestObject(Ray ray, testObj* sphere){
	double a = 1;
	double b = 2*(ray.direction.x*(ray.origin.x-sphere->pos1.x)+ray.direction.y*(ray.origin.y-sphere->pos1.y)+ray.direction.z*(ray.origin.z-sphere->pos1.z));
	double c = pow(ray.origin.x - sphere->pos1.x, 2) + pow(ray.origin.y - sphere->pos1.y, 2) + pow(ray.origin.z - sphere->pos1.z, 2) - pow(sphere->rad.totalRadius,2);
	
	double disc = pow(b,2)-4*c;

	double tempa = 1;
	double tempb = 2*(ray.direction.x*(ray.origin.x-sphere->pos2.x)+ray.direction.y*(ray.origin.y-sphere->pos2.y)+ray.direction.z*(ray.origin.z-sphere->pos2.z));
	double tempc = pow(ray.origin.x - sphere->pos2.x, 2) + pow(ray.origin.y - sphere->pos2.y, 2) + pow(ray.origin.z - sphere->pos2.z, 2) - pow(sphere->rad.totalRadius,2);
	
	double tempdisc = pow(tempb,2)-4*c;

	double t0;
	double t1;

	double t3;
	double t4;

	t0 = (((-1)*b-sqrt(disc))/2);
	t1 = (((-1)*b+sqrt(disc))/2);

	t3 = (((-1)*b-sqrt(tempdisc))/2);
	t4 = (((-1)*b+sqrt(tempdisc))/2);

	if (disc < 0 || tempdisc < 0){
		//no intersection
		return -1;
	} else{
		if (t0>0){
			return t0;
		} else  if (t1>0){
			return t1;
		} else if(t3>0){
			return t3;
		} else if (t4>0){
			return t4;
		}
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
	double a = dotProduct(plane->normal,ray.origin);
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

//Triangles

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

double getTriangleDistance(Triangle* t){
	double distance;
	distance = dotProduct(t->normal, t->A);
	return distance;
}

Position getNormalAt(Triangle t){
	return t.normal;
}

//Last Triangle attempt

double triangleIntersect(Ray ray, Triangle* t){
	Position tempA;
	Position tempB;
	Position N;
	Position P;
	Position C;
	Position edge0;
	Position VP0;
	Position edge1;
	Position VP1;
	Position edge2;
	Position VP2;

	Position CA;
	Position CP;
	double NdotRayDirection;
	double d;
	double tempT;

	t->A = normalize(t->A);
	t->B = normalize(t->B);
	t->C = normalize(t->C);
	t->normal = getTriangleNormal(t->A,t->B,t->C);

	tempA.x = t->B.x - t->A.x;
	tempA.y = t->B.y - t->A.y;
	tempA.z = t->B.z - t->A.z;

	tempB.x = t->C.x - t->A.x;
	tempB.y = t->C.y - t->A.y;
	tempB.z = t->C.z - t->A.z;

	N = crossProd(tempA, tempB);
	NdotRayDirection = dotProduct(N, ray.direction);
	if (NdotRayDirection == 0){
		return -1;
	}
	d = dotProduct(N, t->A);
	tempT = -(((dotProduct(N, ray.origin)) + t->distance) / NdotRayDirection);
	printf("Temp t %lf",tempT);
	printf("\n");
	if (tempT < 0){
		return -1;
	}
	//point P = rayOrig + t * rayDir;
	P.x = ray.origin.x + tempT * ray.direction.x;
	P.y = ray.origin.y + tempT * ray.direction.y;
	P.z = ray.origin.z + tempT * ray.direction.z;
	
	edge0.x = t->B.x - t->A.x;
	edge0.y = t->B.y - t->A.y;
	edge0.z = t->B.z - t->A.z;

	VP0.x = P.x - t->A.x;
	VP0.y = P.y - t->A.y;
	VP0.z = P.z - t->A.z;

	C = crossProd(edge0, VP0);
	printf("dotprod n and c edge 1 %lf",dotProduct(N, C));
	printf("\n");
	if (dotProduct(N, C) < 0){
		return -1;
	}

	edge1.x = t->C.x - t->B.x;
	edge1.y = t->C.y - t->B.y;
	edge1.z = t->C.z - t->B.z;

	VP1.x = P.x - t->B.x;
	VP1.y = P.y - t->B.y;
	VP1.z = P.z - t->B.z;

	C = crossProd(edge0, VP1);
	printf("dotprod n and c edge 2 %lf",dotProduct(N, C));
	printf("\n");
	if (dotProduct(N,C) < 0){
		return -1;
	}

	edge2.x = t->A.x - t->C.x;
	edge2.y = t->A.y - t->C.y;
	edge2.z = t->A.z - t->C.z;

	VP2.x = P.x - t->C.x;
	VP2.y = P.y - t->C.y;
	VP2.z = P.z - t->C.z;

	CA = crossProd(C,t->A);
	CP = crossProd(C,P);

	C = crossProd(CA,CP);
	printf("dotprod n and c edge 3 %lf",dotProduct(N, C));
	printf("\n");
	if (dotProduct(N,C) < 0){
		return -1;
	}

	else {
		return 1;
	}
}

//Triangle stuff
double findTriangleIntersection(Ray ray,Triangle* t){	
	double a;
	t->A = normalize(t->A);
	t->B = normalize(t->B);
	t->C = normalize(t->C);
	t->normal = getTriangleNormal(t->A,t->B,t->C);
	/*printf("t normal sadf x %lf",t->normal.x);
	printf("\n");
	printf("t normal sadf y %lf",t->normal.y);
	printf("\n");
	printf("t normal sadf z %lf",t->normal.z);
	printf("\n");*/
	t->distance = getTriangleDistance(t);

	a = dotProduct(ray.direction,t->normal);
	if (a == 0){//ray is parrallel to plane	
		return -1;
	}else{
		double b;
		double distanceToPlane;
		double Qx, Qy, Qz;
		double test1, test2, test3;
		Position Q;
		Position temp;
		Position multTemp;
		Position CA;
		Position QA;
		Position BC;
		Position QC;
		Position AB;
		Position QB;
		temp = ray.origin;
		multTemp = multPositions(t->normal,t->distance);
		multTemp = negative(multTemp);
		temp = addPositions(t->normal, temp);
		b = dotProduct(t->normal, temp);

		distanceToPlane = -1*b/a;

		Qx = (multPositions(ray.direction, distanceToPlane)).x + ray.origin.x;
		Qy = (multPositions(ray.direction, distanceToPlane)).y + ray.origin.y;
		Qz = (multPositions(ray.direction, distanceToPlane)).z + ray.origin.z;

		Q.x = Qx;
		Q.y = Qy;
		Q.z = Qz;

		Q = normalize(Q);

		/*printf("\n");
		printf("Q.x %lf", Q.x);
		printf("\n");
		printf("Q.y %lf", Q.y);
		printf("\n");
		printf("Q.z %lf", Q.z);
		printf("\n");*/

		// [CAxQA]*n>=0
		CA.x = t->C.x - t->A.x;
		CA.y = t->C.y - t->A.y;
		CA.z = t->C.z - t->A.z;

		QA.x = Q.x - t->A.x;
		QA.y = Q.y - t->A.y;
		QA.z = Q.z - t->A.z;

		CA = normalize(CA); //normalize
		QA = normalize(QA); //normalize

		test1 = dotProduct(crossProd(CA, QA),t->normal);	

		// [BCxQC]*n>=0
		BC.x = t->B.x - t->C.x;
		BC.y = t->B.y - t->C.y;
		BC.z = t->B.z - t->C.z;

		QC.x = Q.x - t->C.x;
		QC.y = Q.y - t->C.y;
		QC.z = Q.z - t->C.z;

		BC = normalize(BC); //normalize
		QC = normalize(QC); //normalize

		test2 = dotProduct(crossProd(BC, QC),t->normal);	

		// [ABxQB]*n>=0
		AB.x = t->A.x - t->B.x;
		AB.y = t->A.y - t->B.y;
		AB.z = t->A.z - t->B.z;

		QB.x = Q.x - t->B.x;
		QB.y = Q.y - t->B.y;
		QB.z = Q.z - t->B.z;

		AB = normalize(AB); //normalize
		QB = normalize(QB); //normalize

		test3 = dotProduct(crossProd(AB, QB),t->normal);

		/*printf("\n");
		printf("test1 %lf", test1);
		printf("\n");
		printf("test2 %lf", test2);
		printf("\n");
		printf("test3 %lf", test3);
		printf("\n");*/
		//test if inside triangle
		if ((test1 >= 0) && (test2 >= 0) && (test3 >= 0)){
			//inside triangle
			return -1*b/a;
		} else {
			return -1;//outside triangle
		}		
	}
}

/*double findTriangleIntersection(Ray ray,Triangle* t){
	

	double a = dotProduct(ray.direction,t->normal);
	if (a == 0){//ray is parrallel to plane	
		return -1;
	}else{
		double b;
		double distanceToPlane;
		double Qx, Qy, Qz;
		double test1, test2, test3;
		Position Q;
		Position temp;
		Position multTemp;
		Position CA;
		Position QA;
		Position BC;
		Position QC;
		Position AB;
		Position QB;
		temp = ray.origin;
		multTemp = multPositions(t->normal,t->distance);
		multTemp = negative(multTemp);
		temp = addPositions(t->normal, temp);
		b = dotProduct(t->normal, temp);

		distanceToPlane = -1*b/a;

		Qx = (multPositions(ray.direction, distanceToPlane)).x + ray.origin.x;
		Qy = (multPositions(ray.direction, distanceToPlane)).y + ray.origin.y;
		Qz = (multPositions(ray.direction, distanceToPlane)).z + ray.origin.z;

		Q.x = Qx;
		Q.y = Qy;
		Q.z = Qz;

		// [CAxQA]*n>=0
		CA.x = t->C.x - t->A.x;
		CA.y = t->C.y - t->A.y;
		CA.z = t->C.z - t->A.z;

		QA.x = Q.x - t->A.x;
		QA.y = Q.y - t->A.y;
		QA.z = Q.z - t->A.z;

		test1 = dotProduct(crossProd(CA, QA),t->normal);	

		// [BCxQC]*n>=0
		BC.x = t->B.x - t->C.x;
		BC.y = t->B.y - t->C.y;
		BC.z = t->B.z - t->C.z;

		QC.x = Q.x - t->C.x;
		QC.y = Q.y - t->C.y;
		QC.z = Q.z - t->C.z;

		test2 = dotProduct(crossProd(BC, QC),t->normal);	
		// [ABxQB]*n>=0
		AB.x = t->A.x - t->B.x;
		AB.y = t->A.y - t->B.y;
		AB.z = t->A.z - t->B.z;

		QB.x = Q.x - t->B.x;
		QB.y = Q.y - t->B.y;
		QB.z = Q.z - t->B.z;

		test3 = dotProduct(crossProd(AB, QB),t->normal);

		printf("\n");
		printf("test1 %lf", test1);
		printf("\n");
		printf("test2 %lf", test2);
		printf("\n");
		printf("test3 %lf", test3);
		printf("\n");

		//test if inside triangle
		if ((test1 >= 0) && (test2 >= 0) && (test3 >= 0)){
			//inside triangle
			return -1*b/a;
		} else {
			return -1;//outside triangle
		}		
	}
}*/


double calculateAmbient(double colour, double lightSourceColour, double Ia, double Ra){
	double ret;
	ret =  Ia * Ra;

	return colour * Ra;
}

double calculateDiffuse(double colour, Position lightToHitPoint, Position hitPointNormal, Position lightSourcePos, double lightSourceColour, double Is, double Rd){
	double a;
	double ret;

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

double calculateSpecular(double colour, Position lightRay, Position reflectionRay, double lightSourceColour, double Is, double Rs, double f){
	double ret;
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
	double total = colour.r + colour.g + colour.b;
	double extra = total - 255 * 3;

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

int findClosestIntersectionPoint(Ray cameraRay, int x, int y){
	double currentClosestZ;
	int returnId;
	double intersectionT;
	double intersectionZVal;
	Sphere *testSphere;
	Plane *testPlane;
	Triangle *testTriangle;

	testSphere = global.sph;
	testPlane = global.pla;
	testTriangle = global.tri;

	returnId = -1;
	currentClosestZ = 10000000;

	while(testSphere){
		intersectionT = testFindSphere(cameraRay, testSphere);
		
		if(intersectionT > 0){
			intersectionZVal = cameraRay.origin.z + cameraRay.direction.z*intersectionT;
			if(intersectionZVal < currentClosestZ){
				currentClosestZ = intersectionZVal;
				returnId = testSphere->id;
			}
		}
		testSphere = testSphere->next;
	}

	while(testPlane){
		intersectionT = findPlaneIntersection(cameraRay, testPlane);

		if(intersectionT > 0){
			intersectionZVal = cameraRay.origin.z + cameraRay.direction.z*intersectionT;
			if(intersectionZVal < currentClosestZ){
				currentClosestZ = intersectionZVal;
				returnId = testPlane->id;
			}
		}
		testPlane = testPlane->next;
	}

	while(testTriangle){
		intersectionT = findTriangleIntersection(cameraRay, testTriangle);

		if(intersectionT > 0){
			intersectionZVal = cameraRay.origin.z + cameraRay.direction.z*intersectionT;
			if(intersectionZVal < currentClosestZ){
				currentClosestZ = intersectionZVal;
				returnId = testTriangle->id;
			}
		}
		testTriangle = testTriangle->next;
	}

	return returnId;
}

void rayTrace(pixel* Im){
	//Ray declaration stuff
	int i,j;
	int x,y,z;
	int closestId;
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
	Position cam_ray_origin;
	Position cam_ray_direction;
	Ray camera_ray;
	Sphere *testSphere;
	Plane *testPlane;
	Triangle *testTriangle;
	Triangle *nextTriangletest;
	testObj *testObject;
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

	campos.x = screenWidth/2;
	campos.y = screenWidth/2;
	campos.z = -10;

	look_at.x = 0;
	look_at.y = 0;
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

	light_pos.x = -7;
	light_pos.y = 10;
	light_pos.z = -10;

	planeColor.r = 0.5;
	planeColor.g = 0.25;
	planeColor.b = 0.25;

	readFile();

	aspectratio = (double)screenWidth/(double)screenWidth;

	//testobj

	testObject = (testObj*)malloc(sizeof (struct testObj));

	/*testObject->pos1.x = 400;	
	testObject->pos1.y = 300;
	testObject->pos1.z = 500;*/

	testObject->pos1.x = 400;	
	testObject->pos1.y = 300;
	testObject->pos1.z = 500;

	testObject->pos2.x = 500;	
	testObject->pos2.y = 300;
	testObject->pos2.z = 500;

	testObject->rad.totalRadius = 10;

	testObject->col.r = 255;
	testObject->col.g = 0;
	testObject->col.b = 0;



	//Triangle stuff

	//triangle->A = normalize(triangle->A);
	//triangle->B = normalize(triangle->B);
	//triangle->C = normalize(triangle->C);

	//triangle->normal = getTriangleNormal(triangle->A,triangle->B,triangle->C);

	//triangle->distance = getTriangleDistance(triangle);

	/*printf("normal of triangle x %lf",triangle->normal.x);
	printf("\n");
	printf("normal of triangle y %lf",triangle->normal.y);
	printf("\n");
	printf("normal of triangle z %lf",triangle->normal.z);
	printf("\n");*/

	//printf("distance of triangle %lf",triangle->distance);
	for (i = 0;i<screenWidth;i++){
		for (j = 0;j<screenWidth;j++){		
			double intersectionT;
			double r,g,b;
			double r2,g2,b2;						

			//dlete me
			double testobjectvalue;
			//dlete


			Position p;
			Position direction;
			Position raySphereIntersection;			
			Colour calcColour;

			p.x = i;
			p.y = j;			
			p.z = 1000;
			
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
			
			//triangleIntersection = triangleIntersect(camera_ray, triangle);

			/*if (triangleIntersection > 0){
				Im[i+j*screenWidth].r = 0;
				Im[i+j*screenWidth].b = 255;
				Im[i+j*screenWidth].g = 0;
			}*/

			testSphere = global.sph;
			testPlane = global.pla;
			testTriangle = global.tri;

			closestId = findClosestIntersectionPoint(camera_ray, i, j);

			while(testSphere){
				if(testSphere->id == closestId){
					break;
				}
				testSphere = testSphere->next;
			}

			while(testPlane){
				if(testPlane->id == closestId){
					break;
				}
				testPlane = testPlane->next;
			}

			while(testTriangle){
				if(testTriangle->id == closestId){
					break;
				}
				testTriangle = testTriangle->next;
			}

			if(testSphere){
				intersectionT = testFindSphere(camera_ray, testSphere);
				raySphereIntersection.x = camera_ray.origin.x + camera_ray.direction.x*intersectionT;
				raySphereIntersection.y = camera_ray.origin.y + camera_ray.direction.y*intersectionT;
				raySphereIntersection.z = camera_ray.origin.z + camera_ray.direction.z*intersectionT;
					
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
						/*
						reflectionRay = getReflectionRay(campos, raySphereIntersection, sphereNormal);
						r2 += calculateSpecular(testSphere->col.r, lightVector, reflectionRay, lightSource->col.r, lightSource->Is, testSphere->eff.Rs, testSphere->eff.f);
						g2 += calculateSpecular(testSphere->col.g, lightVector, reflectionRay, lightSource->col.g, lightSource->Is, testSphere->eff.Rs, testSphere->eff.f);
						b2 += calculateSpecular(testSphere->col.b, lightVector, reflectionRay, lightSource->col.b, lightSource->Is, testSphere->eff.Rs, testSphere->eff.f);
						*/

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
			}
			else if(testPlane){				
				calcColour.r = testPlane->col.r;
				calcColour.g = testPlane->col.g;
				calcColour.b = testPlane->col.b;
			}
			else if(testTriangle){
				calcColour.r = testTriangle->col.r;
				calcColour.g = testTriangle->col.g;
				calcColour.b = testTriangle->col.b;
			}
			else{
				calcColour.r = 0;
				calcColour.g = 0;
				calcColour.b = 0;
			}			
			Im[i+j*screenWidth].r = calcColour.r;					
			Im[i+j*screenWidth].g = calcColour.g;
			Im[i+j*screenWidth].b = calcColour.b;
		}
			testobjectvalue = findIntersectionTestObject(camera_ray, testObject);
			if (testobjectvalue>0){
				Im[i+j*screenWidth].r = 255;					
				Im[i+j*screenWidth].g = 0;
				Im[i+j*screenWidth].b = 0;
			}
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
	
	rayTrace(global.data);
	write_img("test.jpg", global.data, screenWidth, screenWidth);	

}