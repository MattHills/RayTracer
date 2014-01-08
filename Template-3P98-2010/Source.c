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
	int currentId;
	double prevRefra;
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

//double findIntersectionTestObject(Ray ray, testObj* sphere){
	/*double a = (pow(ray.direction.x,2) + pow(ray.direction.y,2));
	double b = 2*((ray.origin.x)*(ray.direction.x))+2*((ray.origin.y)*(ray.direction.y));
	double c = pow(ray.origin.x, 2) + pow(ray.origin.y, 2) - 1;*/
	
	/*double a = (pow(ray.direction.z,2) + pow(ray.direction.y,2));
	double b = 2*((ray.origin.z)*(ray.direction.z))+2*((ray.origin.y)*(ray.direction.y));
	double c = pow(ray.origin.z, 2) + pow(ray.origin.y, 2) - 1;*/
	/*double s = 1;

	double a,b,c,d,f;

	double disc;
	
	double t0;
	double t1;

	d = (s-1)*ray.direction.x;

	f = 1 +(s-1)*ray.origin.z;

	a = (pow(ray.direction.x,2) + pow(ray.direction.y,2) - pow(d,2));
	b = ray.origin.x*ray.direction.x+ray.origin.y*ray.direction.y - f*d;
	c = pow(ray.origin.x,2) + pow(ray.origin.y,2) - pow(f,2);

	disc = pow((2*b),2) - 4*a*c;

	//disc = pow(b,2)-4*c;

	t0 = (((-1)*b-sqrt(disc))/2*a);
	t1 = (((-1)*b+sqrt(disc))/2*a);

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
}*/

double findIntersectionTestObject(Ray ray, testObj* cylinder){
	double a;
	double b;
	double c;
	double delta;

	a = ray.direction.x* ray.direction.x + ray.direction.y * ray.direction.y;
	b = 2 * (ray.origin.x * ray.direction.x + ray.origin.y * ray.direction.y);
	c = (ray.origin.x * ray.origin.x + ray.origin.y * ray.origin.y) - cylinder->rad.totalRadius;

	delta = b * b - (4 * a * c);
	if (delta > 0){
		return -1;
	}
	else if (delta == 0){
		return (-b / (2 * a));
	}
	return 10000;
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
	Position planeNormal;
	Position rayOrigin;
	double a;
	//Calculate Plane normal Position dot Ray origin
	planeNormal = normalize(plane->normal);
	rayOrigin = normalize(ray.origin);
	a = dotProduct(planeNormal,ray.origin);
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


Position getSphereNormal(Sphere *sphere, Position hitPoint){
	Position v;	
	v.x = (hitPoint.x - sphere->pos.x) / sphere->rad.totalRadius;
	v.y = (hitPoint.y - sphere->pos.y) / sphere->rad.totalRadius;
	v.z = (hitPoint.z - sphere->pos.z) / sphere->rad.totalRadius;	
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

	ret.x = (iF * eyeRay.x) + (iF * dotProd - temp) * normal.x;
	ret.y = (iF * eyeRay.y) + (iF * dotProd - temp) * normal.y;
	ret.z = (iF * eyeRay.z) + (iF * dotProd - temp) * normal.z;

	return ret;
}

int findClosestIntersectionPoint(Ray cameraRay){
	double currentClosestZ;
	int returnId;
	double intersectionT;
	double intersectionZVal;
	Sphere *testSphere;
	Plane *testPlane;
	Triangle *testTriangle;
	int a;

	testSphere = global.sph;
	testPlane = global.pla;
	testTriangle = global.tri;

	returnId = -1;
	currentClosestZ = 10000000;

	while(testSphere && testSphere->id != global.currentId){
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
	while(testPlane && testPlane->id != global.currentId){
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

	while(testTriangle && testTriangle->id != global.currentId){
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

double square(double x){
	return x * x;
}

int findClearPath(Ray hitPointVector, LightSource *lightSource, int closestId){
	Sphere *testSphere;
	Plane *testPlane;
	Triangle *testTriangle;
	double intersectionT;
	Ray normalized;
	double distance;
	Position intersection;

	testSphere = global.sph;
	testPlane = global.pla;
	testTriangle = global.tri;
	normalized.direction = normalize(hitPointVector.direction);
	normalized.origin = hitPointVector.origin;
	// Perhaps reverse the orgin/hitpoint if the light is in a different direction
	while(testSphere){
		if(testSphere->id != closestId){
			intersectionT = testFindSphere(normalized, testSphere);
			intersection.x = hitPointVector.origin.x + hitPointVector.direction.x * intersectionT;
			intersection.y = hitPointVector.origin.y + hitPointVector.direction.y * intersectionT;
			intersection.z = hitPointVector.origin.z + hitPointVector.direction.z * intersectionT;
			//intersectionT = testFindSphere(hitPointVector, testSphere);
			if(intersectionT > 0){
				distance = sqrt(square(hitPointVector.origin.x - hitPointVector.direction.x) + square(hitPointVector.origin.y - hitPointVector.direction.y));
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
	
	/*
	while(testTriangle){
		intersectionT = findTriangleIntersection(hitPointVector, testTriangle);
		if(intersectionT > 0){
			return 0;
		}
		testTriangle = testTriangle->next;
	}
	*/
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

	//dlete me
	double testobjectvalue;
	//dlete
	Position direction;	
	Position planeNormal;
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
		intersectionT = findPlaneIntersection(ray, testPlane);
		rayIntersection.x = ray.origin.x + ray.direction.x*intersectionT;
		rayIntersection.y = ray.origin.y + ray.direction.y*intersectionT;
		rayIntersection.z = ray.origin.z + ray.direction.z*intersectionT;

		calcNormal = normalize(testPlane->normal);

		transparency = testPlane->eff.trans;
		If = testPlane->eff.If;
		Ra = testPlane->eff.Ra;
		Rd = testPlane->eff.Rd;
		Rs = testPlane->eff.Rs;
		f = testPlane->eff.f;

		origColour.r = testPlane->col.r;
		origColour.g = testPlane->col.g;
		origColour.b = testPlane->col.b;

		calcColour.r = testPlane->col.r;
		calcColour.g = testPlane->col.g;
		calcColour.b = testPlane->col.b;

		hitPoint = 1;
	}
	else if(testTriangle){
		intersectionT = findTriangleIntersection(ray, testTriangle);				
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

			if(clearPath == 1){					
				// Diffuse Reflection Calculation
				r2 += calculateDiffuse(origColour.r, lightVector, calcNormal, lightSource->pos, lightSource->col.r, lightSource->Is, Rd);
				g2 += calculateDiffuse(origColour.g, lightVector, calcNormal, lightSource->pos, lightSource->col.g, lightSource->Is, Rd);
				b2 += calculateDiffuse(origColour.b, lightVector, calcNormal, lightSource->pos, lightSource->col.b, lightSource->Is, Rd);
						
				// Specular Calculation
						
				reflectionRay.origin = rayIntersection;
				reflectionRay.direction = getReflectionRay(ray.origin, rayIntersection, calcNormal);
				r2 += calculateSpecular(origColour.r, lightVector, reflectionRay.direction, lightSource->col.r, lightSource->Is, Rs, f);
				g2 += calculateSpecular(origColour.g, lightVector, reflectionRay.direction, lightSource->col.g, lightSource->Is, Rs, f);
				b2 += calculateSpecular(origColour.b, lightVector, reflectionRay.direction, lightSource->col.b, lightSource->Is, Rs, f);
						
				// Reflection Calculation
				
				if(Rs > 0.5){
					traceColour = rayTrace(reflectionRay);
					r2 += traceColour.r;
					g2 += traceColour.g;
					b2 += traceColour.b;
				}
				
				// Refraction Calculation

				if(transparency == 1){
					double n;
					double tempPrevRefra;
					Ray refractedRay;

					n = global.prevRefra /  If;
					refractedRay.direction = calcRefraction(ray.origin, rayIntersection, calcNormal, n);
					refractedRay.origin = rayIntersection;

					tempPrevRefra = global.prevRefra;
					global.prevRefra = If;
					traceColour = rayTrace(refractedRay);			
					global.prevRefra = tempPrevRefra;

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
	int i,j,k;
	double x,y,z;
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
	double fovx, fovy;
	double thisone;
	int aadepth = 1;
	double aathreshold = 0.1;

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

	//testobj
	
	testObject = (testObj*)malloc(sizeof (struct testObj));

	testObject->pos.x = 400;	
	testObject->pos.y = 300;
	testObject->pos.z = 400;

	testObject->rad.totalRadius = 50;

	testObject->pos = normalize(testObject->pos);
	//triangle->B = normalize(triangle->B);
	//triangle->C = normalize(triangle->C);

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
			Colour calcColour;

			thisone = j*screenWidth + i;

			//cam_ray_origin = camera.campos;
			//cam_ray_direction = addPositions(camdir, multPositions(camright, xamnt - 0.5)



			//x = ((2*i - screenWidth)/screenWidth) * tan(fovx);
			//y = ((2*j - screenWidth)/screenWidth) * tan(fovy);
			
			innerTemp.x = 0;
			innerTemp.y = 0;
			innerTemp.z = 0;

			innerTemp2.x = 0;
			innerTemp2.y = 0;
			innerTemp2.z = 0;

			innerTemp3.x = 0;
			innerTemp3.y = 0;
			innerTemp3.z = 0;

			
			xamount = (screenWidth - i +0.5)/screenWidth;
			yamount = ((screenWidth-j)+0.5)/screenWidth;
			innerTemp = multPositions(camdown, yamount - 0.5);
			innerTemp2 = multPositions(camright, xamount - 0.5);
			innerTemp2 = addPositions(innerTemp2, innerTemp);
			innerTemp2 = addPositions(camdir, innerTemp2);
			innerTemp2 = normalize(innerTemp2);
			
			camera_ray.origin = camera.campos;
			camera_ray.direction = innerTemp2;
			
			/*cam_ray_origin = camera.campos;
			innerTemp3 = camright;
			innerTemp3 = multPositions(innerTemp3,(xamount-0.5));
			innerTemp = camdir;
			innerTemp = addPositions(innerTemp, innerTemp3);
			innerTemp2 = camdown;
			innerTemp3 = multPositions(innerTemp2,(yamount-0.5));
			innerTemp2 = normalize(innerTemp2);
			innerTemp = addPositions(innerTemp, innerTemp2);
			camera_ray.origin = cam_ray_origin;
			*/
			//camera_ray.direction = innerTemp;

			/*direction.x = p.x - camera_ray.origin.x;
			direction.y = p.y - camera_ray.origin.y;
			direction.z = p.z - camera_ray.origin.z;

			direction = normalize(direction);		

			camera_ray.direction = direction;
			*/
			
			//triangleIntersection = triangleIntersect(camera_ray, triangle);

			/*if (triangleIntersection > 0){
				Im[i+j*screenWidth].r = 0;
				Im[i+j*screenWidth].b = 255;
				Im[i+j*screenWidth].g = 0;
			}*/

			global.currentId = -1;

			calcColour = rayTrace(camera_ray);			
			
			Im[i+j*screenWidth].r = calcColour.r;					
			Im[i+j*screenWidth].g = calcColour.g;
			Im[i+j*screenWidth].b = calcColour.b;
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