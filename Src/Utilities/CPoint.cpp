// CPoint.cpp -- n-dimensional point class implementation
//
// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from it's use.
// Users of this code must verify correctness for their application.
//
// Extended by Stephen Pope -- see the CSL copyright notice

#include "CPoint.h"

using namespace csl;

// CPoint Class Methods

CPoint::CPoint(PointMode m, float tr, float ttheta) {
	dimn=2;
	if (m == kCartesian) {
		x = (double)tr;
		y = (double)ttheta;
		z = 0;
	} else {			// else Polar
		x = tr * cosf(ttheta);
		y = tr * sinf(ttheta);
		z = 0;
	}
}

CPoint::CPoint(PointMode m, float tr, float ttheta, float tphi) {
	dimn=3;
	if (m == kCartesian) {
		x = (double)tr;
		y = (double)ttheta;
		z = (double)tphi;
	} else {			// else Polar
		x = (tr * cosf(ttheta) * cosf(tphi));
		y = (tr * sinf(ttheta) * cosf(tphi));
		z = (tr * sinf(tphi));
	}
}

//CPoint::CPoint(CPoint & other) {
//	dimn=other.dimn;
//	x = other.x;
//	y = other.y;
//	z = other.z;
//}

// Assign (set) dimn

unsigned CPoint::setdim(unsigned n) {
	dimn = n;
	switch (n) {
		case 1: y = 0;
		case 2: z = 0;
	}
	return dimn;
}

void CPoint::set(PointMode m, float a, float b) { 
	dimn=2; 
	if (m == kCartesian) {
		x = a;
		y = b;
		z = 0;
	} else {			// else Polar
		x = a * cosf(b);
		y = a * sinf(b);
		z = 0;
	}
}
void CPoint::set(PointMode m, float a, float b, float c) { 
	dimn=3; 
	if (m == kCartesian) {
		x = a;
		y = b;
		z = c;
	} else {			// else Polar
		x = a * cosf(b) * cosf(c);
		y = a * sinf(b) * cosf(c);
		z = a * sinf(c);
	}
}

void CPoint::setAzimuth(double taz) {
	float tr = this->len();
	x=tr * cosf(taz); 
	y=tr * sinf(taz);
}

void CPoint::setElevation(double tele) {
	float tr = this->len();
	float ttheta = this->theta();
	x=tr * cosf(ttheta) * cosf(tele); 
	y=tr * sinf(ttheta) * cosf(tele); 
	z=tr * sinf(tele);
}

void CPoint::setMagnitude(double tmag) {
	float tr = tmag / this->len();
	x *= tr; 
	y *= tr; 
	if (dimn== 3)
		z *= tr; 
}

//------------------------------------------------------------------
//  Unary Ops
//------------------------------------------------------------------

// Unary minus

CPoint CPoint::operator-() {
	CPoint v;
	v.x = -x; v.y = -y; v.z = -z;
	v.dimn = dimn;
	return v;
}

// Unary 2D perp operator

CPoint CPoint::operator~() {
//	if (dimn != 2) err = Edim;   // and continue anyway
	CPoint v;
	v.x = -y; v.y = x; v.z = z;
	v.dimn = dimn;
	return v;
}

// Comparison (note: dimension must compare)

int CPoint::operator==(CPoint Q) {
	if (dimn != Q.dim()) return FALSE;
	switch (dimn) {
	case 1:
		return (x==Q.x);
	case 2:
		return (x==Q.x && y==Q.y);
	case 3:
	default:
		return (x==Q.x && y==Q.y && z==Q.z);
	}
}

int CPoint::operator!=(CPoint Q) {
	if (dimn != Q.dim()) return TRUE;
	switch (dimn) {
	case 1:
		return (x!=Q.x);
	case 2:
		return (x!=Q.x || y!=Q.y);
	case 3:
	default:
		return (x!=Q.x || y!=Q.y || z!=Q.z);
	}
}

// CPoint Vector Operations

CPoint CPoint::operator+(CPoint v) {
	CPoint P;
	P.x = x + v.x;
	P.y = y + v.y;
	P.z = z + v.z;
	P.dimn = csl_max(dimn, v.dim());
	return P;
}

CPoint CPoint::operator-(CPoint v) {
	CPoint P;
	P.x = x - v.x;
	P.y = y - v.y;
	P.z = z - v.z;
	P.dimn = csl_max(dimn, v.dim());
	return P;
}

//------------------------------------------------------------------
//  Products
//------------------------------------------------------------------

// Inner Dot Product
COORD_TYPE CPoint::operator*(CPoint w ) {
	return (x * w.x + y * w.y + z * w.z);
}

// 2D Exterior Perp Product
COORD_TYPE CPoint::operator|(CPoint w ) {
//	if (dimn != 2) err = Edim;    // and continue anyway
	return (x * w.y - y * w.x);
}

// 3D Exterior Cross Product
CPoint CPoint::operator^(CPoint w ) {
	CPoint v;
	v.x = y * w.z - z * w.y;
	v.y = z * w.x - x * w.z;
	v.z = x * w.y - y * w.x;
	v.dimn = 3;
	return v;
}

CPoint& CPoint::operator*=(double c ) {        // vector scalar mult
	x *= c;
	y *= c;
	z *= c;
	return *this;
}

CPoint& CPoint::operator/=(double c ) {        // vector scalar div
	x /= c;
	y /= c;
	z /= c;
	return *this;
}

CPoint& CPoint::operator+=(CPoint v) {
	x += v.x;
	y += v.y;
	z += v.z;
	dimn = csl_max(dimn, v.dim());
	return *this;
}

CPoint& CPoint::operator-=(CPoint v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	dimn = csl_max(dimn, v.dim());
	return *this;
}

CPoint& CPoint::operator^=(CPoint w ) {        // 3D exterior cross product
	double ox=x, oy=y, oz=z;
	x = oy * w.z - oz * w.y;
	y = oz * w.x - ox * w.z;
	z = ox * w.y - oy * w.x;
	dimn = 3;
	return *this;
}

// CPoint Scalar Operations (convenient but often illegal)

CPoint operator*(int c, CPoint Q) {
	CPoint P;
	P.x = c * Q.x;
	P.y = c * Q.y;
	P.z = c * Q.z;
	P.dimn = Q.dim();
	return P;
}

CPoint operator*(float c, CPoint Q) {
	CPoint P;
	P.x = c * Q.x;
	P.y = c * Q.y;
	P.z = c * Q.z;
	P.dimn = Q.dim();
	return P;
}

CPoint operator*(double c, CPoint Q) {
	CPoint P;
	P.x = c * Q.x;
	P.y = c * Q.y;
	P.z = c * Q.z;
	P.dimn = Q.dim();
	return P;
}

CPoint operator*(CPoint Q, int c) {
	CPoint P;
	P.x = c * Q.x;
	P.y = c * Q.y;
	P.z = c * Q.z;
	P.dimn = Q.dim();
	return P;
}

CPoint operator*(CPoint Q, float c) {
	CPoint P;
	P.x = c * Q.x;
	P.y = c * Q.y;
	P.z = c * Q.z;
	P.dimn = Q.dim();
	return P;
}

CPoint operator*(CPoint Q, double c) {
	CPoint P;
	P.x = c * Q.x;
	P.y = c * Q.y;
	P.z = c * Q.z;
	P.dimn = Q.dim();
	return P;
}

CPoint operator/(CPoint Q, int c) {
	CPoint P;
	P.x = Q.x / c;
	P.y = Q.y / c;
	P.z = Q.z / c;
	P.dimn = Q.dim();
	return P;
}

CPoint operator/(CPoint Q, float c) {
	CPoint P;
	P.x = (float)(Q.x / c);
	P.y = (float)(Q.y / c);
	P.z = (float)(Q.z / c);
	P.dimn = Q.dim();
	return P;
}

CPoint operator/(CPoint Q, double c) {
	CPoint P;
	P.x = Q.x / c;
	P.y = Q.y / c;
	P.z = Q.z / c;
	P.dimn = Q.dim();
	return P;
}

// CPoint Addition (also convenient but often illegal)

CPoint operator+(CPoint Q, CPoint R) {
	CPoint P;
	P.x = Q.x + R.x;
	P.y = Q.y + R.y;
	P.z = Q.z + R.z;
	P.dimn = csl_max(Q.dim(), R.dim());
	return P;
}

COORD_TYPE CPoint::operator () (unsigned idx) const {
	switch(idx) {
		case 0: 
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			return 0;
	}
}

// Distance between CPoints

COORD_TYPE CPoint::distance(CPoint * Q) {      // Euclidean distance
	COORD_TYPE dx = x - Q->x;
	COORD_TYPE dy = y - Q->y;
	COORD_TYPE dz = z - Q->z;
	return sqrtf(dx*dx + dy*dy + dz*dz);
}

COORD_TYPE CPoint::distance2(CPoint * Q) {     // squared distance (more efficient)
	COORD_TYPE dx = x - Q->x;
	COORD_TYPE dy = y - Q->y;
	COORD_TYPE dz = z - Q->z;
	return (dx*dx + dy*dy + dz*dz);
}

COORD_TYPE CPoint::distance(CPoint & Q) {      // Euclidean distance
	COORD_TYPE dx = x - Q.x;
	COORD_TYPE dy = y - Q.y;
	COORD_TYPE dz = z - Q.z;
	return sqrtf(dx*dx + dy*dy + dz*dz);
}

COORD_TYPE CPoint::distance2(CPoint & Q) {     // squared distance (more efficient)
	COORD_TYPE dx = x - Q.x;
	COORD_TYPE dy = y - Q.y;
	COORD_TYPE dz = z - Q.z;
	return (dx*dx + dy*dy + dz*dz);
}

// Polar operations -- answer the magnitude and angle of a point

COORD_TYPE CPoint::theta() {
    if (x == 0.0) {
	    if (y >= 0.0) {
			return CSL_PI * 0.5;
	    }
	    else {
			return CSL_PI * 1.5;
	    }
    }
	COORD_TYPE tan = atanf(y / x);
	if (x < 0)
		return (tan + CSL_PI);
	else if (tan < 0.0)
		return (tan + CSL_TWOPI);
	else
		return (tan);
}

// phi = atan (sqrt (x**2 + y**2) / z)

COORD_TYPE CPoint::phi() {
	if (z == 0.0)
		return CSL_PI * 0.5;
	double tan = atanf(sqrtf((x*x) + (y*y)) / z);
	return (tan);
}

COORD_TYPE CPoint::ele() {
	return asin(z / len() );
}

// Rotate the receiver by the argument (in radians)

void CPoint::rotateBy(double angle) {
	COORD_TYPE mag = len();
	COORD_TYPE t = theta() + angle;
	x = mag * cosf(t); 
	y = mag * sinf(t);
}

void CPoint::normalize() {                      // convert to unit length
	COORD_TYPE len = sqrt(x*x + y*y + z*z);
	if (len == 0) return;						// do nothing for nothing
	x /= len;
	y /= len;
	z /= len;
}

