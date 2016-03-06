// wire3d.hpp

#ifndef WIRE3D_HPP
 #define WIRE3D_HPP
 
#include <allegro.h> 
#include "matrix.hpp"

#ifndef M_PI
 #define M_PI 3.1415926535897932384626433832795
 #define M_2_PI 6.28318530717958647692528676655901
 #define M_05_PI 1.57079632679489661923132169163975
#endif

// global variables -----------------------------------------------------------+
extern void (*draw_pixel)(BITMAP *, int, int, int);
extern int color;
extern int color_min;
extern float falloff;

// Vector class ---------------------------------------------------------------+
class Vector
{
 public:
        // constructors / destructor:
        Vector(): x(0.0f), y(0.0f), z(0.0f) { }
        Vector(float a, float b, float c): x(a), y(b), z(c) { }
        Vector(const Vector &v): x(v.x), y(v.y), z(v.z) { }
        ~Vector() { }

        // operators:
        Vector operator+ (const Vector &v) { return Vector(x+v.x, y+v.y, z+v.z); }
        Vector operator- (const Vector &v) { return Vector(x-v.x, y-v.y, z-v.z); }
        void operator += (const Vector &v) { x+=v.x; y+=v.y; z+=v.z; }
        void operator -= (const Vector &v) { x-=v.x; y-=v.y; z-=v.z; }
        
        Vector operator* (float v) { return Vector(x*v, y*v, z*v); }
        Vector operator/ (float v) { if (v == 0.0f) v = 1.0f; return Vector(x/v, y/v, z/v); }
        void operator*= (float v) { x*=v; y*=v; z*=v; }
        void operator/= (float v) { if (v == 0.0f) v = 1.0f/v; x*=v; y*=v; z*=v; }
        
        operator V3D_f() { V3D_f v = { x, y, z, 0.0f, 0.0f, 0 }; return v; }               
        
        // variables:
        float x;
        float y;
        float z;
 private:
}; 

// function prototypes --------------------------------------------------------+
void WireBox(BITMAP *bmp, float sx, float sy, float sz);
void WirePlane(BITMAP *bmp, float size, int w, int h);
void WireCylinder(BITMAP *bmp, float r, float h, int faces, int parts, int top = 1, int bottom = 1);
 
#endif // WIRE3D_HPP

// end of file ----------------------------------------------------------------+ 
