#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "line.h"
#include "plane.h"
#include "bbox.h"
#include "bsphere.h"
#include "intersect.h"
#include "trfm3D.h"

////// WARNING !!! IREJECT == 1 // IINTERSECT == 0
void pause() {
	printf("Press ENTER to continue...\n");
	getchar();
}

// Vector3
void printVector3(const char *text, const Vector3 &v) {
	printf("      %s", text); v.print(); printf("\n");
}

Vector3 getVector3(const char *text) {
	float vx, vy, vz;

	printf(" %s:", text); scanf("%f, %f, %f", &vx, &vy, &vz); printf("\n");

	getchar();

	return Vector3(vx, vy, vz);
}


// Bounding Box
void printBBox(const char *text, const BBox *bb) {
	printf("      %s:", text); bb->print(); printf("\n");
}

BBox * getBBox(const char *text) {
	float xmin, ymin, zmin, xmax, ymax, zmax;
	BBox *bb;

	printf("\n  %s", text);
	printf("\n"); printf("  Min (xmin, ymin, zmin):"); scanf("%f, %f, %f", &xmin, &ymin, &zmin);
	printf("\n"); printf("  Max (xmax, ymax, zmax):"); scanf("%f, %f, %f", &xmax, &ymax, &zmax);
	printf("\n");

	getchar();

	bb = new BBox(Vector3(xmin, ymin, zmin), Vector3(xmax, ymax, zmax));

	return bb;
}

// Plane
void printPlane (const char *text, const Plane *p) {
	printf("      %s:", text);
	printf("  Normal nx, ny, nz: %f, %f, %f  d: %f", p->m_n[0], p->m_n[1], p->m_n[2], p->m_d);
	printf("\n");
}

Plane * getPlane (const char *text) {
	float nx, ny, nz, d;
	Plane *p;

	printf("\n  %s", text);
	printf("\n"); printf("  Normal nx, ny, nz:"); scanf("%f, %f, %f", &nx, &ny, &nz);
	printf("\n"); printf("  d:"); scanf("%f", &d);
	printf("\n");

	getchar();

	p = new Plane(Vector3(nx, ny, nz), d);

	return p;
}

// Sphere
void printSphere (const char *text, const BSphere *bs) {
	printf("      %s:", text);
	printf("  Center cx, cy, cz: %f, %f, %f  Radius: %f", bs->m_centre[0], bs->m_centre[1], bs->m_centre[2], bs->m_radius);
	printf("\n");
}

BSphere * getSphere (const char *text) {
	float cx, cy, cz, r;
	BSphere *bs;

	printf("\n  %s", text);
	printf("\n"); printf("  Center cx, cy, cz:"); scanf("%f, %f, %f", &cx, &cy, &cz);
	printf("\n"); printf("  Radius r:"); scanf("%f", &r);
	printf("\n");

	getchar();

	bs = new BSphere(Vector3(cx, cy, cz), r);

	return bs;
}

//***************************************************
//***
//*** Distance from Point to Line
//***
//***************************************************
void caseLine1(const Vector3 & A, const Vector3 & B, const Vector3 & P) {
	Line linea;

	// Line
	linea.setFromAtoB(A, B);

	float u = linea.paramDistance(P);
	float d = linea.distance(P);

	printf("\n");
	printf(" Result for line:\n");
	printVector3("A", A); printVector3("B", B); printVector3("Point", P);
	printf("\n The line: ");
	linea.print();
	printf("      u = %.3f       distance = %.3f\n", u, d);
	pause();
}

void caseLine2() {
	Line l;

	// Line
	Vector3 O = getVector3("O");
	Vector3 D = getVector3("Dx, Dy, Dz:");
	l.setFromAtoB(O, D);

	// Point
	Vector3 P = getVector3("Px, Py, Pz:");

	float u = l.paramDistance(P);
	float d = l.distance(P);

	printf("\n");
	printf(" Result for line:\n");
	printVector3("O", O); printVector3("D", D); printVector3("Point", P);
	printf("\n The line: ");
	l.print();
	printf("      u = %.3f       distance = %.3f\n", u, d);

	pause();
}

void menuLine() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test Distance from point to line\n");
		printf ("**************************************\n\n");

		printf(" 1. Line from A(0,0,0) to B(1,1,0) and Point(0,1,0)\n");
		printf(" 2. Line from A(0,0,0) to B(1,1,0) and Point(0.5,0.5,0.5)\n");
		printf(" 3. Line from A(5,3,1) to B(1,4,-2) and Point(1.7692, 3.8077, -1.4231)\n");
		printf(" 4. Line from A(5,3,1) to B(1,4,-2) and Point(1,4,-3)\n");
		printf(" 5. Enter data\n");
		printf(" 6. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%d", &option); getchar();

		switch( option ) {
		case 1:
			caseLine1(Vector3(0.0, 0.0, 0.0),
					  Vector3(1.0, 1.0, 0.0),
					  Vector3(0.0, 1.0, 0.0));
			break;
		case 2:
			caseLine1(Vector3(0.0, 0.0, 0.0),
					  Vector3(1.0, 1.0, 0.0),
					  Vector3(0.5, 0.5, 0.5));
			break;
		case 3:
			caseLine1(Vector3(5.0, 3.0, 1.0),
					  Vector3(1.0, 4.0, -2.0),
					  Vector3(1.7692, 3.8077, -1.4231));
			break;
		case 4:
			caseLine1(Vector3(5.0, 3.0, 1.0),
					  Vector3(1.0, 4.0, -2.0),
					  Vector3(1.0, 4.0, -3.0));
			break;
		case 5:
			caseLine2();
			break;
		case 6:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** CASE INTERSECT AABB and AABB
//***
//***************************************************
void caseAABBs(const BBox *bb1, const BBox *bb2) {
	int result;

	result = BBoxBBoxIntersect(bb1, bb2);

	printf("\n Result for BBox-BBox:\n");
	printBBox("BBox1", bb1);
	printBBox("BBox2", bb2);
	printf("      Intersect = %s\n", intersect_string(result));
}

void caseAABBs1(const Vector3 & min1, const Vector3 & max1, const Vector3 & min2, const Vector3 & max2) {
	// First AABB
	BBox *bb1 = new BBox(min1, max1);

	// Second AABB
	BBox *bb2 = new BBox(min2, max2);

	caseAABBs(bb1, bb2);

	delete bb1;
	delete bb2;
	pause();
}

void caseAABBs2() {
	// First AABB
	BBox *bb1 = getBBox("BBox1");

	// Second AABB
	BBox *bb2 = getBBox("BBox2");

	caseAABBs(bb1, bb2);

	delete bb1;
	delete bb2;
	pause();

}

void menuAABBs() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect AABB-AABB\n");
		printf ("**************************************\n\n");

		printf(" 1. AABB{(0,0,0),(1,1,1)} and AABB{(0,0,0),(2,2,2)}\n");
		printf(" 2. AABB{(0,0,0),(1,1,1)} and AABB{(1.5,1.5,1.5),(2.5,2.5,2.5)}\n");
		printf(" 3. AABB{(0.9, 0.9, 0.9),(1.9, 1.9, 1.9)} and AABB{(0,0,0),(2,2,2)}\n");
		printf(" 4. AABB{(-1,-1,-1),(1,1,1)} and AABB{(0,0,0),(2,2,2)}\n");
		printf(" 5. AABB{(1, 2.5, 1),(2, 3.5, 2)} and AABB{(1,1,1),(2,2,2)}\n");
		printf(" 6. Enter data\n");
		printf(" 7. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseAABBs1(Vector3(0.0, 0.0, 0.0),
					   Vector3(1.0, 1.0, 1.0),
					   Vector3(0.0, 0.0, 0.0),
					   Vector3(2.0, 2.0, 2.0));
			break;
		case 2:
			caseAABBs1(Vector3(0.0, 0.0, 0.0),
					   Vector3(1.0, 1.0, 1.0),
					   Vector3(1.5, 1.5, 1.5),
					   Vector3(2.5, 2.5, 2.5));
			break;
		case 3:
			caseAABBs1(Vector3(0.9, 0.9, 0.9),
					   Vector3(1.9, 1.9, 1.9),
					   Vector3(0.0, 0.0, 0.0),
					   Vector3(2.0, 2.0, 2.0));
			break;
		case 4:
			caseAABBs1(Vector3(-1.0, -1.0, -1.0),
					   Vector3(1.0, 1.0, 1.0),
					   Vector3(0.0, 0.0, 0.0),
					   Vector3(2.0, 2.0, 2.0));
			break;
		case 5:
			caseAABBs1(Vector3(1.0, 2.5, 1.0),
					   Vector3(2.0, 3.5, 2.0),
					   Vector3(1.0, 1.0, 1.0),
					   Vector3(2.0, 2.0, 2.0));
			break;
		case 6:
			caseAABBs2();
			break;
		case 7:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** CASE INTERSECT AABB and Plane
//***
//***************************************************
void caseAABBplane(const BBox *bb, Plane *p) {
	int result;

	result = BBoxPlaneIntersect(bb, p);

	printf("\n");
	printf(" Result for AABB-plane:\n");
	printBBox("BBox", bb);
	printPlane("Plane", p);
	printf("      Intersect = %s\n", intersect_string(result));
}

void caseAABBplane1(const Vector3 & vmin, const Vector3 & vmax, const Vector3 & n, float d) {
	BBox *bb;
	Plane *p;

	bb = new BBox(vmin, vmax);
	p  = new Plane(n, d);

	caseAABBplane(bb, p);

	delete bb;
	delete p;
	pause();

}

void caseAABBplane2() {
	BBox *bb;
	Plane *p;

	// AABB
	bb = getBBox("BBox");

	// Plane
	p  = getPlane("Plane");

	caseAABBplane(bb, p);

	delete bb;
	delete p;
	pause();

}

void menuAABBplane() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect AABB-Plane\n");
		printf ("**************************************\n\n");

		printf(" 1. AABB{(0,0,0),(1,1,1)} and Plane{(4,-4,0), 0}\n");
		printf(" 2. AABB{(-2,0,0),(-1,1,1)} and Plane{(4,-4,0), 0}\n");
		printf(" 3. AABB{(0,-1.5,0.5),(1,-0.5,1.5)} and Plane{(4,-4,0), 0}\n");
		printf(" 4. AABB{(-1,-1,-2),(0,0,-1)} and Plane{(4,-4,0), 0}\n");
		printf(" 5. AABB{(2,0,0),(3,1,1)} and Plane{(4,-4,0), 0}\n");
		printf(" 6. AABB{(-0.5,0,0),(0.5,1,1)} and Plane{(4,-4,0), 0}\n");
		printf(" 7. Enter data\n");
		printf(" 8. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseAABBplane1(Vector3(0.0, 0.0, 0.0),
						   Vector3(1.0, 1.0, 1.0),
						   Vector3(4.0, -4.0, 0.0),
						   0.0);
			break;
		case 2:
			caseAABBplane1(Vector3(-2.0, 0.0, 0.0),
						   Vector3(-1.0, 1.0, 1.0),
						   Vector3(4.0, -4.0, 0.0),
						   0.0);
			break;
		case 3:
			caseAABBplane1(Vector3(0.0, -1.5, 0.5),
						   Vector3(1.0, -0.5, 1.5),
						   Vector3(4.0, -4.0, 0.0),
						   0.0);
			break;
		case 4:
			caseAABBplane1(Vector3(-1.0, -1.0, -2.0),
						   Vector3(0.0, 0.0, -1.0),
						   Vector3(4.0, -4.0, 0.0),
						   0.0);
			break;
		case 5:
			caseAABBplane1(Vector3(2.0, 0.0, 0.0),
						   Vector3(3.0, 1.0, 1.0),
						   Vector3(4.0, -4.0, 0.0),
						   0.0);
			break;
		case 6:
			caseAABBplane1(Vector3(-0.5, 0.0, 0.0),
						   Vector3(0.5, 1.0, 1.0),
						   Vector3(4.0, -4.0, 0.0),
						   0.0);
			break;
		case 7:
			caseAABBplane2();
			break;
		case 8:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** CASE INTERSECT between SPHERES
//***
//***************************************************
void caseSpheres(const BSphere *bs1, const BSphere *bs2) {
	int result;

	result = BSphereBSphereIntersect(bs1, bs2);

	printf("\n");
	printf(" Result for Sphere-Sphere:\n");
	printSphere("Sphere1", bs1);
	printSphere("Sphere2", bs2);
	printf("      Intersect = %s\n", intersect_string(result));
}

void caseSpheres1(const Vector3 & c1, float r1, const Vector3 & c2, float r2) {
	BSphere *bs1, *bs2;

	bs1 = new BSphere(c1, r1);
	bs2 = new BSphere(c2, r2);

	caseSpheres(bs1, bs2);

	delete bs1;
	delete bs2;
	pause();

}

void caseSpheres2() {
	BSphere *bs1, *bs2;

	bs1 = getSphere("Sphere1");
	bs2 = getSphere("Sphere2");

	caseSpheres(bs1, bs2);

	delete bs1;
	delete bs2;

	pause();
}

void menuSpheres() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect Sphere-Sphere\n");
		printf ("**************************************\n\n");

		printf(" 1. Sphere1{(0,0,0), 1.0} and Sphere2{(0.5,1,1), 1.0}\n");
		printf(" 2. Sphere1{(0,0,0), 1.0} and Sphere2{(-2.5,1,0), 1.0}\n");
		printf(" 3. Sphere1{(0,0,0), 1.0} and Sphere2{(0,0,0), 1.5}\n");
		printf(" 4. Sphere1{(0,0,0), 1.0} and Sphere2{(2.5,1,0), 1.0}\n");
		printf(" 5. Sphere1{(0,0,0), 0.5} and Sphere2{(0,0,0), 1.0}\n");
		printf(" 6. Enter data\n");
		printf(" 7. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseSpheres1(Vector3(0.0, 0.0, 0.0), 1.0,
						 Vector3(0.5, 1.0, 1.0), 1.0);
			break;
		case 2:
			caseSpheres1(Vector3(-2.5, 1.0, 0.0), 1.0,
						 Vector3(1.0, 1.0, 1.0), 1.0);
			break;
		case 3:
			caseSpheres1(Vector3(0.0, 0.0, 0.0), 1.0,
						 Vector3(0.0, 0.0, 0.0), 1.5);
			break;
		case 4:
			caseSpheres1(Vector3(0.0, 0.0, 0.0), 1.0,
						 Vector3(2.5, 1.0, 0.0), 1.0);
			break;
		case 5:
			caseSpheres1(Vector3(0.0, 0.0, 0.0), 0.5,
						 Vector3(0.0, 0.0, 0.0), 1.0);
			break;
		case 6:
			caseSpheres2();
			break;
		case 7:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** CASE SPHERE-PLANE
//***
//***************************************************
void caseSpherePlane(const BSphere *bs, Plane *p) {
	int result;

	result = BSpherePlaneIntersect(bs, p);
	printf("\n");
	printf(" Result for Sphere-Plane:\n");
	printSphere("Sphere", bs);
	printPlane("Plane", p);
	printf("      Intersect = %s\n", intersect_string(result));
}

void caseSpherePlane1 (const Vector3 & c, float r, const Vector3 & n, float d) {
	BSphere *bs;
	Plane *p;

	bs = new BSphere(c, r);
	p  = new Plane(n, d);

	caseSpherePlane(bs, p);

	delete bs;
	delete p;
	pause();

}

void caseSpherePlane2 (){
	BSphere *bs;
	Plane *p;

	// Sphere
	bs = getSphere("Sphere");

	// Plane
	p  = getPlane("Plane");

	caseSpherePlane(bs, p);

	delete bs;
	delete p;
	pause();

}

void menuSpherePlane() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect Sphere-Plane\n");
		printf ("**************************************\n\n");

		printf(" 1. Sphere{(0,0,0), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 2. Sphere{(2,0.3,-0.5), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 3. Sphere{(-1.5,0.3,0), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 4. Sphere{(0.5,0.5,-1.5), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 5. Sphere{(2,0.5,0.5), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 6. Enter data\n");
		printf(" 7. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseSpherePlane1(Vector3(0.0, 0.0, 0.0), 1.0,
							 Vector3(4.0, -4.0, 0.0), 0.0);
			break;
		case 2:
			caseSpherePlane1(Vector3(2.0, 0.3, -0.5), 1.0,
							 Vector3(4.0, -4.0, 0.0), 0.0);
			break;
		case 3:
			caseSpherePlane1(Vector3(-1.5, 0.3, 0.0), 1.0,
							 Vector3(4.0, -4.0, 0.0), 0.0);
			break;
		case 4:
			caseSpherePlane1(Vector3(0.5, 0.5, -1.5), 1.0,
							 Vector3(4.0, -4.0, 0.0), 0.0);
			break;
		case 5:
			caseSpherePlane1(Vector3(2.0, 0.5, 0.5), 1.0,
							 Vector3(4.0, -4.0, 0.0), 0.0);
			break;
		case 6:
			caseSpherePlane2();
			break;
		case 7:
			end = 1;
			break;

		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}


//***************************************************
//***
//*** CASE INTERSECT Sphere and AABB
//***
//***************************************************
void caseSphereAABB(const BSphere *bs, const BBox *bb) {
	int result;

	result = BSphereBBoxIntersect(bs, bb);
	printf("\n");
	printf(" Result for Sphere-AABB:\n");
	printSphere("Sphere", bs);
	printBBox("AABB", bb);
	printf("      Intersect = %s\n", intersect_string(result));

}

void caseSphereAABB1(const Vector3 & c, float r, const Vector3 & Pmin, const Vector3 & Pmax) {
	BSphere *bs;
	BBox *bb;

	bs = new BSphere(c, r);
	bb = new BBox(Pmin, Pmax);

	caseSphereAABB(bs, bb);

	delete bs;
	delete bb;
	pause();
}

void caseSphereAABB2() {
	BBox *bb;
	BSphere *bs;

	// Sphere
	bs = getSphere("Sphere");

	// AABB
	bb = getBBox("AABB");

	caseSphereAABB(bs, bb);

	delete bs;
	delete bb;

	pause();
}

void menuSphereAABB() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect Sphere-AABB\n");
		printf ("**************************************\n\n");

		printf(" 1. Sphere{(0,0,0),1} and AABB{(0,0,0),(1,1,1)}\n");
		printf(" 2. Sphere{(0,0,0),1} and AABB{(-1,-1,-1),(1,1,1)}\n");
		printf(" 3. Sphere{(0,0,0),1} and AABB{(-3,0,0),(-2,1,1)}\n");
		printf(" 4. Sphere{(0,0,0),2} and AABB{(0,0,0),(1,1,1)}\n");
		printf(" 5. Sphere{(0,0,0),1} and AABB{(1,1,1),(2,2,2)}\n");
		printf(" 6. Enter data\n");
		printf(" 7. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseSphereAABB1(Vector3(0.0, 0.0, 0.0), 1.0,
							Vector3(0.0, 0.0, 0.0),
							Vector3(1.0, 1.0, 1.0));
			break;
		case 2:
			caseSphereAABB1(Vector3(0.0, 0.0, 0.0), 1.0,
							Vector3(-1.0, -1.0, -1.0),
							Vector3(1.0, 1.0, 1.0));
			break;
		case 3:
			caseSphereAABB1(Vector3(0.0, 0.0, 0.0), 1.0,
							Vector3(-3.0, 0.0, 0.0),
							Vector3(-2.0, 1.0, 1.0));
			break;
		case 4:
			caseSphereAABB1(Vector3(0.0, 0.0, 0.0), 2.0,
							Vector3(0.0, 0.0, 0.0),
							Vector3(1.0, 1.0, 1.0));
			break;
		case 5:
			caseSphereAABB1(Vector3(0.0, 0.0, 0.0), 1.0,
							Vector3(1.0, 1.0, 1.0),
							Vector3(2.0, 2.0, 2.0));
			break;
		case 6:
			caseSphereAABB2();
			break;
		case 7:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

void printTrfmResult(const std::string & text,
					 const Vector3 &X, const Vector3 &Y,
					 const std::string &result) {
	printf("%s\n", text.c_str());
	printf("Good result: %s\n", result.c_str());
	printf("Your result: ");Y.print(); printf("\n");
	pause();
}

void menuTrfm() {
	int end = 0;
	int option;
	Vector3 X, Y;
	Trfm3D T;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test transformations\n");
		printf ("**************************************\n\n");

		printf(" 1. Point{1,1,1}, Tr(1,1,1)\n");
		printf(" 2. Point{1,1,1}, Tr(1,1,1)*R_x(-pi/2)\n");
		printf(" 3. Point{1,1,1}, Tr(1,1,1)*R_x(-pi/2)*S(1/2,1/2,1/2)\n");
		printf(" 4. Vector{1,1,1}, Tr(1,1,1)\n");
		printf(" 5. Vector{1,1,1}, Tr(1,1,1)*R_x(-pi/2)\n");
		printf(" 6. Vector{1,1,1}, Tr(1,1,1)*R_x(-pi/2)*S(1/2,1/2,1/2)\n");
		printf(" 7. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			X = Vector3(1,1,1);
			T.setTrans(Vector3(1,1,1));
			Y = T.transformPoint(X);
			//Result:(2.0000, 2.0000, 2.0000)
			printTrfmResult("Point{1,1,1}; Transf: Tr(1,1,1)", X, Y, "(2.0000, 2.0000, 2.0000)");
			break;
		case 2:
			X = Vector3(1,1,1);
			T.setTrans(Vector3(1,1,1));
			T.addRotX(-Constants::pi/2.0);
			Y = T.transformPoint(X);
			// Result:(2.0000, 2.0000, 0.0000)
			printTrfmResult("Point{1,1,1}; Transf: Tr(1,1,1)*R_x(-pi/2)", X, Y, "(2.0000, 2.0000, 0.0000)");
			break;
		case 3:
			X = Vector3(1,1,1);
			T.setTrans(Vector3(1,1,1));
			T.addRotX(-Constants::pi/2.0);
			T.addScale(0.5);
			Y = T.transformPoint(X);
			//Result:(1.5000, 1.5000, 0.5000)
			printTrfmResult("Point{1,1,1}; Transf: Tr(1,1,1)*R_x(-pi/2)*S(1/2,1/2,1/2)", X, Y, "(1.5000, 1.5000, 0.5000)");
			break;
		case 4:
			X = Vector3(1,1,1);
			T.setTrans(Vector3(1,1,1));
			Y = T.transformVector(X);
			// Result:(1.0000, 1.0000, 1.0000)
			printTrfmResult("Vector{1,1,1}; Transf: Tr(1,1,1)", X, Y, "(1.0000, 1.0000, 1.0000)");
			break;
		case 5:
			X = Vector3(1,1,1);
			T.setTrans(Vector3(1,1,1));
			T.addRotX(-Constants::pi/2.0);
			Y = T.transformVector(X);
			// Result:(1.0000, 1.0000, -1.0000)
			printTrfmResult("Vector{1,1,1}; Transf: Tr(1,1,1)*R_x(-pi/2)", X, Y, "(1.0000, 1.0000, -1.0000)");
			break;
		case 6:
			X = Vector3(1,1,1);
			T.setTrans(Vector3(1,1,1));
			T.addRotX(-Constants::pi/2.0);
			T.addScale(0.5);
			Y = T.transformVector(X);
			// Result:(0.5000, 0.5000, -0.5000)
			printTrfmResult("Vector{1,1,1}; Transf: Tr(1,1,1)*R_x(-pi/2)*S(1/2,1/2,1/2)", X, Y, "(0.5000, 0.5000, -0.5000)");
			break;
		case 7:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 7\n");
			break;
		}
	}
}



//***************************************************
//***
//*** MENU PRINCIPAL
//***
//***************************************************
void mainMenu() {
	int end = 0;
	int option;

	while (end == 0){
		system("clear");
		printf(" 1. Distance from point to  line\n");
		printf(" 2. Test intersect AABB-AABB\n");
		printf(" 3. Test intersect AABB-plane\n");
		printf(" 4. Test intersect Spheres\n");
		printf(" 5. Test intersect Sphere-plane\n");
		printf(" 6. Test intersect Sphere-AABB\n");
		printf(" 7. Test point and vector transformations\n");
		printf(" 8. Exit\n\n");
		printf(" Enter your choice: ");
		scanf("%d", &option); getchar();

		switch(option) {
		case 1:
			menuLine();
			break;
		case 2:
			menuAABBs();
			break;
		case 3:
			menuAABBplane();
			break;
		case 4:
			menuSpheres();
			break;
		case 5:
			menuSpherePlane();
			break;
		case 6:
			menuSphereAABB();
			break;
		case 7:
			menuTrfm();
			break;
		case 8:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 7\n");
			break;
		}
	}
}


int main() {
	mainMenu();
	return 0;
}
