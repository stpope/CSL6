//////////////////////////////////////////////////////
// Matrix TCL Pro 2.12 Test Program
// Copyright (c) 2000-2003 Techsoft Pvt. Ltd. All Rights Reserved.
// Web: http://www.techsoftpl.com/matrix/
// Email: matrix@techsoftpl.com
//
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "cmatrix"

#if defined(__BORLANDC__)
#pragma option -w-aus
#endif

//
// Using declarations. 
//
using std::cout;
using std::endl;
using techsoft::epsilon;
using techsoft::isVecEq;
using std::exception;

#ifdef _MSC_VER
using ::rand;
#else
using std::rand;
using std::size_t;
#endif


//
// typedef for different matrix types
//
typedef techsoft::matrix<float>        fMatrix;
typedef std::valarray<float>           fVector;

typedef techsoft::matrix<double>       dMatrix;
typedef std::valarray<double>          dVector;

typedef techsoft::matrix<long double>  ldMatrix;
typedef std::valarray<long double>     ldVector;

typedef std::complex<float>            fComplex;
typedef techsoft::matrix<fComplex>     cfMatrix;
typedef std::valarray<fComplex>        cfVector;

typedef std::complex<double>           dComplex;
typedef techsoft::matrix<dComplex>     cdMatrix;
typedef std::valarray<dComplex>        cdVector;

typedef std::complex<long double>      ldComplex;
typedef techsoft::matrix<ldComplex>    cldMatrix;
typedef std::valarray<ldComplex>       cldVector;

const char *TestFile = "test.txt";

void test_const ();     // Test constructor/destructor
void test_submat ();    // Test sub-matrix operations
void test_op ();        // Test unary operators
void test_util ();      // Test utility methods
void test_dcomp ();     // Test decomposition methods
void test_eigen ();     // Test eigen value and eigen vector
void test_io ();		   // Test I/O

template <class T> T 
mfn (size_t i, size_t j, const T& v)
{
   int k = int(i+j);
   return pow( v, k);
}


int main ()
{
   #if !defined(_MSC_VER)
   using std::remove;
   using std::srand;
   #endif

   try
   {
      srand( 0x23657876);
      test_const();
      test_submat();
      test_op();
      test_util();
      test_dcomp();
      test_eigen();
      test_io();
      remove( TestFile);
   }
   catch (const exception& e)
   {
      cout << "Error: " << e.what();
   }
   return 0;
}

void test_const ()     // Test constructor/destructor
{
   try
   {
      cout << "Testing constructors.....";
      dMatrix m(3,3);
      dMatrix m2(3,3,1.0);

      m = 1.0;
      if (m != m2)
      {
         cout << "failed!\n";
         return;
      }

      float fv[] = { 1.34f, 2.54f, 8.23f, 
                     7.34f, -2.3f, 2.45f,
                     -1.2f, 4.50f, 7.34f };

      fMatrix mf( 3, 3, fv);
      fMatrix mf2( 3, 3, fv, techsoft::FORTRAN_ARRAY);
      fMatrix mf3 = mf;
      fVector vf( fv,9);
      fMatrix mf4( 3,3,vf);

      if (mf != ~mf2 || mf != mf3 || mf != mf4)
      {
         cout << "failed!\n";
         return;
      }

      dMatrix ma[5];
      for (size_t i=0; i < 5; i++)
      {
         ma[i].resize( 3, 3);
         if (!ma[i].isNull())
         {
            cout << "failed!\n";
            return;
         }
      }

      cfMatrix cm(4,4);
      cm.rand();
      cfMatrix cm2 = cm;

#ifndef _TS_NO_MEMBER_TEMPLATES
      dMatrix md = mf;
      cdMatrix mcd = md;
      mcd = 1.0;
      mcd = cm;
#endif
   }
   catch (const exception& e)
   {
      cout << "failed!\nError: " << e.what() << endl;
      return;
   }
   cout << "Ok\n";
}


void test_submat ()
{
   using techsoft::mslice;
   using techsoft::gmslice;
   using techsoft::mswap;
   using techsoft::abs;
   using techsoft::DIAGONAL;
   using techsoft::TRIDIAGONAL;
   using techsoft::UTRIANG;
   using techsoft::LTRIANG;

   try
   {
      cout << "Testing subscript/submatrix.....";

      dMatrix m(3,3);
      m.rand();
      const dMatrix m2 = m;
      double x,y;
      size_t i=0,j=0;

      x = m2[i][j];
      m[i][j] = x;
      y = m[i][j];
      if (abs(x-y) > epsilon(x))
      {
         cout << "failed!\n";
         return;
      }
      x = m(i,j)++;
      y = ++m(i,j);
      if (abs(y-x-2.0) > epsilon(y))
      {
         cout << "failed!\n";
         return;
      }
      x = m(i,j)--;
      y = --m(i,j);
      if (abs(x-y-2.0) > epsilon(x))
      {
         cout << "failed!\n";
         return;
      }
      x = m[i][j]  * m[i+1][j+1];
      x = m2[i][j] / m2[i+1][j+1];
      x = m[i][j]  + m2[i+1][j+1];
      x = m2[i][j] - m[i+1][j+1];

      y = x * m[i][j];
      y = m[i][j] * x;
      y = x / m[i][j];
      y = m[i][j] / x;
      y = x + m[i][j];
      y = m[i][j] + x;
      y = x - m[i][j];
      y = m[i][j] - x;

#if defined(_MSC_VER) && _MSC_VER > 1100     // MS VC++ 5.0 is generating wrong code!!!
      x = m[i][j];
      m[i][j] += m[i+1][j+1];
      m[i][j] -= m[i+1][j+1];
      m[i][j] *= m[i+1][j+1];
      m[i][j] /= m[i+1][j+1];
      y = m[i][j];
      if (abs(y-x) > epsilon(y))
      {
         cout << "failed!\n";
         return;
      }
#endif

      x = -m[i][j];
      y = +m[i][j];
      if (abs(y+x) > epsilon(x+y))
      {
         cout << "failed!\n";
         return;
      }

      dMatrix a(6,6);
      a.rand();
      const dMatrix b = a;

      dMatrix sa = b[mslice(1,1,3,3)];
      a[mslice(1,1,3,3)] = sa;
      if (a != b)
      {
         cout << "failed!\n";
         return;
      }

      a[mslice(2,2,1,3)] = b[mslice(2,2,1,3)];
      if (a != b)
      {
         cout << "failed!\n";
         return;
      }

      a[mslice(1,0,2,3)] *= 10.0;
      a[mslice(1,0,2,3)] /= 10.0;
      if (a != b)
      {
         cout << "failed!\n";
         return;
      }

      a[mslice(1,0,2,3)] += b[mslice(1,0,2,3)];
      a[mslice(1,0,2,3)] -= b[mslice(1,0,2,3)];
      if (a != b)
      {
         cout << "failed!\n";
         return;
      }

      a[mslice(0,0,6,6)] = 1.0;
      dMatrix c(6,6,1.0);
      if (a != c)
      {
         cout << "failed!\n";
         return;
      }

      a.resize(4,4);
      a.rand();
      c = a[mslice(1,1,2,2)];
      c = a[gmslice(DIAGONAL)];
      c = a[gmslice(TRIDIAGONAL)];
      c = a[gmslice(UTRIANG)];
      c = a[gmslice(LTRIANG)];

      c = 1.0;
      c[gmslice(DIAGONAL)] += a;   
      c[gmslice(TRIDIAGONAL)] -= a;      
      c[gmslice(UTRIANG)] *= 12.0;
      c[gmslice(LTRIANG)] /= 12.0;

      dMatrix cm = a[gmslice(UTRIANG)];
      const dMatrix ca = a;
      cm = ca[gmslice(UTRIANG)];

      dMatrix d(4,4), d2(4,4);
      d.rand();
      d2.rand();
      mswap( d[0][0], d[1][1]);
      mswap( d[2], d[3]);
      mswap( d(1), d(2));
      
      mswap( d[0][0], d2[0][0]);
      mswap( d[2], d2[2]);
      mswap( d(1), d2(1));
      mswap( d, d2);      

      a = b;
      dVector v = b[5];
      a[5] = v;
      v = a(3);
      a(3) = v;
      a[2] += v;
      a[2] -= v;
      a[4] *= v;
      a[4] /= v;
      if (a != b)
      {
         cout << "failed!\n";
         return;
      }

      v = b.diag();
      a.diag() = v;

      dVector v2 = b.diag(-2);
      a.diag(-2) = v2;
      v2 = b.diag(2);
      a.diag(2) = v2;
      if (a != b)
      {
         cout << "failed!\n";
         return;
      }
   }
   catch (const exception& e)
   {
      cout << "failed!\nError: " << e.what() << endl;
      return;
   }
   cout << "Ok\n";
}

void test_op ()
{
   try
   {
      cout << "Testing operators.....";
      dMatrix m(6,4);

      m.rand();
      dMatrix m2 = m;
      m2 += m;
      m2 -= m;
      if (m2 != m)
      {
         cout << "failed!\n";
         return;
      }
      double te(1.234567f);
      m2 *= te;
      m2 /= te;
      if (m2 != m)
      {
         cout << "failed!\n";
         return;
      }

      dMatrix a(4,4);
      a.rand();
      dMatrix b = !a;
      a *= b;
      if (!a.isUnit())
      {
         cout << "failed!\n";
         return;
      }
      
      a.rand();
      a /= a;
      if (!a.isUnit())
      {
         cout << "failed!\n";
         return;
      }

      a.resize( 5,3);
      a.rand();
      b = ~a;
      dMatrix c = a * b;
      b = ~b;
      if (a != b)
      {
         cout << "failed!\n";
         return;
      }
      c = a + b;
      b = c - a;
      if (a != b)
      {
         cout << "failed!\n";
         return;
      }

      a.resize(4,4);
      a.rand();
      b = !a;
      c = a * b;
      if (!c.isUnit())
      {
         cout << "failed!\n";
         return;
      }

      double x = 2.045f;
      c = 1/x * a * x;
      if (a != c)
      {
         cout << "failed!\n";
         return;
      }

      c = x * a / x;
      if (a != c)
      {
         cout << "failed!\n";
         return;
      }

      c = x / a;
      b = x * !a;
      if (b != c)
      {
         cout << "failed!\n";
         return;
      }

      dVector v(4), v2(4);
      v = b[0];
      a.unit();
      v2 = v * a;
      v = a * v2;
      if (!isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }
      v2 = v / a;
      v = a / v;
   }
   catch (const exception& e)
   {
      cout << "failed!\nError: " << e.what() << endl;
      return;
   }
   cout << "Ok\n";
}

void test_util ()    // Test utility methods
{
   using techsoft::pow;
   using techsoft::abs;
   using techsoft::floor;
   using techsoft::ceil;
   using techsoft::gmslice;
   using techsoft::UTRIANG;
   using techsoft::LTRIANG;

   try
   {
      cout << "Testing utility methods.....";
      dMatrix m(6,4);

      m.rand();
      dMatrix m2 = m;
      m.unit();
      m.resize( 2, 3);
      m.unit();
      m.resize( 4, 6);
      m.unit();
      m.free();

      m.resize( 4, 4);
      m.rand();
      m2 = m * m * m * m * m * m * m;
      if (m2 != pow( m, 7))
      {
         cout << "failed!\n";
         return;
      }

      m2 = -m;
      m2 = abs( m2);

      m2 = floor( m2);
      if (!m2.isNull() || m2.trace() != 0.0)
      {
         cout << "failed!\n";
         return;
      }
      m2 = ceil( m);

      double val;
      size_t rn;

      m.rand();
      val = m.norm1();
      val = m.norm2();
      val = m.normI();
      val = m.normF();
      val = m.cond();
      val = m.det();
      rn = m.rank();
      val = m.cofact(0,0);

      m2 = m.adj()/m.det();
      if (m2 != !m)
      {
         cout << "failed!\n";
         return;
      }

      m2 = m;
      m2.inv_lu();
      if (m2 != !m)
      {
         cout << "failed!\n";
         return;
      }

      m2 = m;
      m2.inv_sv();
      if (m2 != !m)
      {
         cout << "failed!\n";
         return;
      }

      m2 = m;
      m2.inv_qr();
      if (m2 != !m)
      {
         cout << "failed!\n";
         return;
      }

      cdMatrix cm(4,4), cm2(4,4);
      cm.rand();
      cm2 = cm;
      cm2.inv_sv();
      if (cm2 != !cm)
      {
         cout << "failed!\n";
         return;
      }

      bool bt = m.isSingular();
      m = double(0);
      bt = m.isNull();
      m.diag() = 1.0;
      bt = m.isUnit();
      m.diag() = 3.0;
      bt = m.isScalar();
      m.diag()[0] = 7.0;
      bt = m.isDiagonal();

      m.rand();
      m2 = m[gmslice(UTRIANG)];
      if (!m2.isUpperTriangular())
      {
         cout << "failed!\n";
         return;
      }
      m2 = m[gmslice(LTRIANG)];
      if (!m2.isLowerTriangular())
      {
         cout << "failed!\n";
         return;
      }

      dMatrix m3 = ~m2;
      m2[gmslice(UTRIANG)] = m3;
      if (!m2.isSymmetric())
      {
         cout << "failed!\n";
         return;
      }
      m2[gmslice(UTRIANG)] = -m3;
      if (!m2.isSkewSymmetric())
      {
         cout << "failed!\n";
         return;
      }
#if defined(_MSC_VER) && _MSC_VER > 1100
      m2 = m.apply( log);
      m2 = m.apply( mfn);
#endif
   }
   catch (const exception& e)
   {
      cout << "failed!\nError: " << e.what() << endl;
   }
   cout << "Ok\n";

}

void test_dcomp ()     // Test constructor/destructor
{
   using techsoft::gmslice;
   using techsoft::LTRIANG;

   try
   {
      cout << "Testing matrix decomposition.....";
      size_t i, n = 4;
      dMatrix a(n,n);
      dVector v(n),s(n),v2(n), w(n);
      std::valarray<size_t> indx(n);

      a.rand();
      dMatrix b = a;
      v = a[0];
      v *= double(1.2394f);

      b.lud( indx);
      b.lubksb( indx, v, s);
      v2 = a * s;
      if (!isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      a.lumpove( b, indx, v, s);
      v2 = a * s;
      if (!isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      v /= double(-1.7913f);
      b.lubksb( indx, v, s);
      v2 = a * s;
      if (!isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      dMatrix mv(n,3), ms(n,3), mv2(n,3);
      mv.rand();
      a.solve( mv, ms);
      for (i=0; i < 3; i++)
      {
         dVector vx = ms(i);
         mv2(i) = a * vx;
      }
      if (mv != mv2)
      {
         cout << "failed!\n";
         return;
      }

      b = a[gmslice(LTRIANG)];
      b *= ~b;
      a = b;
      b.chold();
      b.cholbksb( v, s);
      v2 = a * s;
      if (a != (b * ~b) || !isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      a.solve_chol( v, s);
      v2 = a * s;
      if (!isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      a.rand();
      b = a;
      dMatrix c(n,n);
      b.qrd( c);
      b.qrbksb( c, v, s);
      v2 = a * s;
      if (!b.isRowOrthogonal() || a != (b * c) || !isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      a.solve_qr( v, s);
      v2 = a * s;
      if (!isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      b = a;
      b.svd( c, w);
      b.svbksb( c, w, v, s);
      v2 = a * s;
      dMatrix d(n,n,0.0);
      d.diag() = w;
      if (!b.isColOrthogonal() || !c.isRowOrthogonal()
          || a != (b * d * ~c) || !isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      a.solve_sv( v, s);
      v2 = a * s;
      if (!isVecEq( v, v2))
      {
         cout << "failed!\n";
         return;
      }

      a.resize( n+2,n);
      a.rand();
      b = a;
      b.qrd( c);
      v.resize( n+2);
      for (i=0; i < n+2; i++)
         v[i] = ((double)rand())/RAND_MAX;
      b.qrbksb( c, v, s);
      v2 = a * s;
      if (a != b * c)
      {
         cout << "failed!\n";
         return;
      }

      b = a;
      b.svd( c, w);
      b.svbksb( c, w, v, s);
      d.diag() = w;

      if (a != (b * d * ~c)
          || !b.isColOrthogonal() || !c.isRowOrthogonal())
      {
         cout << "failed!\n";
         return;
      }

      cdMatrix ca(n,n);
      cdVector cv(n),cs(n),cv2(n);

      ca.rand();
      for (i=0; i < n; i++)
      {
         double ta = ((double)rand())/RAND_MAX;
         double tb = ((double)rand())/RAND_MAX;
         dComplex z(ta,tb);
         cv[i] = z;
      }

      cdMatrix cb = ca;
      cdMatrix cd = !ca;
      cs = cd * cv;
      cv2 = ca * cs;
      if (!isVecEq( cv, cv2))
      {
         cout << "failed!\n";
         return;
      }

      cb.lud( indx);
      cb.lubksb( indx, cv, cs);
      cv2 = ca * cs;
      if (!isVecEq( cv, cv2))
      {
         cout << "failed!\n";
         return;
      }
   }
   catch (const exception& e)
   {
      cout << "failed!\nError: " << e.what() << endl;
   }
   cout << "Ok\n";
}


void test_eigen ()
{
   using techsoft::gmslice;
   using techsoft::UTRIANG;
   using techsoft::LTRIANG;

   try
   {
      cout << "Testing eigen value.....";

      size_t n = 4;
      dMatrix a(n,n), ev(n,n);
      dVector d(n), e(n);
      bool ret,ret2;

      a.rand();
      a = a[gmslice(UTRIANG)];
      a[gmslice(LTRIANG)] = ~a;

      ret = a.eigen( e);
      ret2 = a.eigen( d, ev);
      if (!ret || !ret2 || !isVecEq( d, e))
      {
         cout << "failed!\n";
         return;
      }

      dMatrix c(n,n,0.0);
      c.diag() = d;
      if ((a * ev) != (ev * c))
      {
         cout << "failed!\n";
         return;
      }

      a.rand();
      ret = a.eigen( d, e);
      ret2 = a.eigen( d, e, ev);
      if (!ret || !ret2)
      {
         cout << "failed!\n";
         return;
      }

      for (size_t i=0; i < n; i++)
      {
         c(i,i) = d[i];
         if (e[i] > double(0) && i < n-1)
            c(i,i+1) = e[i];
         else if (e[i] < double(0) && i > 0)
            c(i,i-1) = e[i];
      }
      if ((a * ev) != (ev * c))
      {
         cout << "failed!\n";
         return;
      }
   }
   catch (const exception& e)
   {
      cout << "failed!\nError: " << e.what() << endl;
   }
   cout << "Ok\n";
}


void test_io ()
{
   using techsoft::abs;
   #if !defined(_MSC_VER)
   using std::FILE;
   using std::fopen;
   using std::fclose;
   using std::fread;
   using std::fwrite;
   #endif

   size_t i,m=4,n=3;
   dMatrix a(m,n), b(m,n);
   dVector v(m), v2(m);
   double tmp;
   cout << "Testing matrix I/O.....";
   a.rand();

   std::ofstream outFile( TestFile);
   if (!outFile)
   {
      std::cerr << "\nError opefing test file.";
	   return;
   }
   outFile.clear();

   std::ifstream inFile( TestFile);
   if (!inFile)
   {
	   std::cerr << "\nError opefing test file.";
	   return;
   }
   inFile.tie( &outFile);
   outFile << std::scientific << std::setprecision( 18);

   outFile << a[0][0] << endl;
   inFile >> b[0][0];
   tmp = a[0][0] - b[0][0];
   if (abs( tmp) > epsilon(tmp))
   {
      cout << "failed!\n" << a[0][0] << endl << b[0][0] ;
      return;
   }

   const dMatrix c = a;
   outFile << c(1,1) << endl;
   inFile >> b(1,1);
   tmp = c(1,1) - b(1,1);
   if (abs( tmp) > epsilon( tmp))
   {
      cout << "failed!\n";
      return;
   }

   v = a(0);
   outFile << a(0) << endl;
   inFile >> a(0);
   v2 = a(0);
   a(0) = v;

   if (!isVecEq( v, v2))
   {
      cout << "failed!\n";
      return;
   }

   outFile << a[1] << endl;
   inFile >> b[1];
   for (i=0; i < n; i++)
   {
      tmp = a[1][i] - b[1][i];
      if (abs( tmp) > epsilon(tmp))
      {
         cout << "failed!\n";
         return;
      }
   }

   outFile << c[1] << endl;
   inFile >> b[1];
   for (i=0; i < n; i++)
   {
      tmp = c[1][i] - b[1][i];
      if (abs( tmp) > epsilon(tmp))
      {
         cout << "failed!\n";
         return;
      }
   }

   outFile << a << endl;
   inFile >> b;
   if (a != b)
   {
      cout << "failed!\n";
      return;
   }
   outFile.close();
   inFile.close();

   FILE *fp = fopen( TestFile, "w+b");
   if (fp == NULL)
   {
      std::cerr << "\nError opefing test file.";
	   return;
   }
   
   a.rand();
   fwrite( &a[0][0], a.size(), a.typesize(), fp);
   rewind( fp);
   fread( &b[0][0], b.size(), b.typesize(), fp);
   fclose( fp);
   if (a != b)
   {
      cout << "failed!\n";
      return;
   }
	cout << "Ok\n";
}

