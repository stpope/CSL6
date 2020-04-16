//
// CPoint.h -- n-dimensional point class specification
//
// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from it's use.
// Users of this code must verify correctness for their application.
//
// Extended by Stephen Pope -- see the CSL copyright notice

#ifndef CSL_Point_H
#define CSL_Point_H

#include "CSL_Types.h"		// TRUE/FALSE
#include <stdio.h>			// printf
#include <math.h>			// trig fcns.

#define COORD_TYPE	float		// type of point members

namespace csl {

#ifdef CSL_ENUMS
typedef enum {				// point types
	kCartesian,
	kPolar
} PointMode;
#else
	#define kCartesian 0
	#define kPolar 1
	typedef int PointMode;
#endif

//  CPoint Class Definition

class CPoint {

public:
	unsigned dimn;		// # dimensions (1, 2, or 3)
	COORD_TYPE x, y, z;		// z = 0 for 2D, y = z = 0 for 1D

						// Lots of Constructors
	CPoint() { dimn = 3; x = y = z = 0;  }
	
	// ~~~~~~~~~ 1D ~~~~~~~~~~~~~~
	CPoint(int a) { dimn = 1; x = (float) a; y = z = 0; }
	CPoint(float a) { dimn = 1; x = a; y = z = 0;  }
	CPoint(double a) { dimn = 1; x = a; y = z = 0;  }
	
	// ~~~~~~~~~ 2D (defaults are Cartesian) ~~~~~~~~~~~~~~
	CPoint(int a, int b) { dimn = 2; x = (float) a; y = (float) b; z = 0;  }
	CPoint(float a, float b) { dimn = 2; x = (COORD_TYPE)a; y = (COORD_TYPE)b; z = 0;  }
	CPoint(double a, double b) { dimn = 2; x = (COORD_TYPE)a; y = (COORD_TYPE)b; z = 0;  }
	
	// ~~~~~~~~~ 3D ~~~~~~~~~~~~~~
	CPoint(int a, int b, int c) { dimn = 3; x = (float) a; y = (float) b; z = c;  }
	CPoint(float a, float b, float c) { dimn = 3; x = (COORD_TYPE)a; y = (COORD_TYPE)b; z = (COORD_TYPE)c;  }
	CPoint(double a, double b, double c) { dimn = 3; x = (COORD_TYPE)a; y = (COORD_TYPE)b; z = (COORD_TYPE)c;  }
	
	// ~~~~~~~~~ 2-D and 3-D polar ~~~~~~~~~~~~~~
	CPoint(PointMode m, float tr, float ttheta);
	CPoint(char s, double tr, double ttheta) { dimn=2; x=tr * cosf(ttheta); y=tr * sinf(ttheta); z=0; }
	
	CPoint(PointMode m, float tr, float ttheta, float psi);
	CPoint(char s, double tr, double ttheta, double tele){ 
			dimn=3; x=tr*cosf(ttheta)*cosf(tele); y=tr*sinf(ttheta)*cosf(tele); z=tr*sinf(tele);}
	
//	CPoint(CPoint & other);		// Copy constructor -- use '=' instead
	~CPoint() { };				// Destructor

	// ~~~~~~~~~ Accessors ~~~~~~~~~~~~~~
	void set(int a, int b) { dimn = 2; x = (float) a; y = (float) b; z = 0;  }
	void set(int a, int b, int c) { dimn = 3; x = (float) a; y = (float) b; z = (float) c;  }
	void set(float a, float b) { dimn = 2; x = (COORD_TYPE)a; y = (COORD_TYPE)b; z = 0;  }
	void set(float a, float b, float c) { dimn = 3; x = (COORD_TYPE)a; y = (COORD_TYPE)b; z = (COORD_TYPE)c;  }
	void set(double a, double b) { dimn = 2; x = (COORD_TYPE)a; y = (COORD_TYPE)b; z = 0;  }
	void set(double a, double b, double c) { dimn = 3; x = (COORD_TYPE)a; y = (COORD_TYPE)b; z = (COORD_TYPE)c;  }
	void set(PointMode m, float a, float b);
	void set(PointMode m, float a, float b, float c);
	void set(char s, double tr, double ttheta) { dimn=2; x=tr * cosf(ttheta); y=tr * sinf(ttheta); z=0; }
	void set(char s, double tr, double ttheta, double tele) { 
			dimn=3; x=tr*cosf(ttheta)*cos(tele); y=tr*sinf(ttheta)*cosf(tele); z=tr*sinf(tele);}

	void setAzimuth(double taz);
	void setElevation(double tele);
	void setMagnitude(double tmag);
	
	unsigned dim() { return dimn; }	// get dimension
	unsigned setdim(unsigned);		// set new dimension
	
	//----------------------------------------------------------
	// CVector Unary Operations
	CPoint operator-();                // unary minus
	CPoint operator~();                // unary 2D perp operator
	
	int operator == (CPoint);
						// Comparison (dimension must match, or not)
	int operator != (CPoint);
						// Point and Vector Operations (always valid) 
//	CPoint operator - (CPoint);	   	// Vector difference
//	CPoint  operator + (CPoint);		// +translate
//	CPoint& operator += (CPoint);     	// inc translate
//	CPoint& operator -= (CPoint);     	// dec translate
	
		//----------------------------------------------------------
	// CVector Arithmetic Operations
	CPoint operator+(CPoint);        // vector add
	CPoint operator-(CPoint);        // vector subtract
	COORD_TYPE operator*(CPoint);    // inner dot product
	COORD_TYPE operator|(CPoint);    // 2D exterior perp product
	CPoint operator^(CPoint);        // 3D exterior cross product

	CPoint& operator*=(double);      // vector scalar mult
	CPoint& operator/=(double);      // vector scalar div
	CPoint& operator+=(CPoint);      // vector increment
	CPoint& operator-=(CPoint);      // vector decrement
	CPoint& operator^=(CPoint);      // 3D exterior cross product

						// CPoint Scalar Operations (convenient but often illegal)
						// Scalar Multiplication
	friend CPoint operator * (int, CPoint);
	friend CPoint operator * (float, CPoint);
	friend CPoint operator * (double, CPoint);
	friend CPoint operator * (CPoint, int);
	friend CPoint operator * (CPoint, float);
	friend CPoint operator * (CPoint, double);
						// Scalar Division
	friend CPoint operator / (CPoint, int);
	friend CPoint operator / (CPoint, float);
	friend CPoint operator / (CPoint, double);
						// CPoint Relations
	COORD_TYPE distance(CPoint *);		// Distance
	COORD_TYPE distance2(CPoint *);  	// Distance^2
	COORD_TYPE distance(CPoint &);		// Distance
	COORD_TYPE distance2(CPoint &);  	// Distance^2
//	COORD_TYPE isLeft(CPoint, CPoint);	// 2D only

   COORD_TYPE operator () (unsigned idx) const;
	
	//----------------------------------------------------------
	// CVector Properties
	COORD_TYPE len() {                    // vector length
		return sqrtf(x*x + y*y + z*z);
	}
	
	COORD_TYPE len2() {                   // vector length squared (faster)
		return (x*x + y*y + z*z);
	}	
						// Polar/Spherical coordinates
	COORD_TYPE r() { return len(); };
	COORD_TYPE theta();
	COORD_TYPE phi();

	COORD_TYPE ele();	
						// 2D Rotation
	void rotateBy(double angle);
						// Pretty-printing
	void dump() { fprintf(stderr, " CP: %g @ %g @ %g", x, y, z); }
	void dumpPol() {
		fprintf(stderr, " CP: %g @ %g @ %g", 
				r(), 
				theta()* CSL_DEGS_PER_RAD, 
				ele()* CSL_DEGS_PER_RAD);
	}

	void normalize();                 // convert vector to unit length

};

}

#endif		// SS_Point_H
