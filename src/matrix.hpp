// matrix.hpp
// Warning! Code is not good! It is sufficient for small transformation...
//
// 20 June 2003
// Fen

#ifndef _MATRIX_HPP
 #define _MATRIX_HPP
 
#include <allegro.h>
#include <math.h> 

// matrix stack ---------------------------------------------------------------+
// size: ( 64 + 12 ) * MAX_MSTACK_DEPTH [bytes]
#define MAX_MSTACK_DEPTH 16                       // maximum of matrices in stack

// function prototypes --------------------------------------------------------+

// managment:
void InitMatrices();     // call it first !
void ResetMatrix();
int PushMatrix();
int PopMatrix();

// transformations:
void TranslateMatrix(float x, float y, float z);
        
void RotateMatrix(float ax, float ay, float az);
void RotateMatrixX(float ax);
void RotateMatrixY(float ay);
void RotateMatrixZ(float az);
        
void ScaleMatrix(float sx, float sy, float sz);

// camara:
void CameraMatrix(float x, float y, float z,
                  float xfront, float yfront, float zfront,
                  float xup, float yup, float zup, float fov, float asp);
void CameraLookAt(float x, float y, float z,
                  float cx, float cy, float cz, float fov, float asp);        
void GetCameraPos(float &x, float &y, float &z);
                  
// final process:

// applies matrix on point        
void ApplyMatrix(float x, float y, float z, float *out_x, float *out_y, float *out_z);               
 
// applies matrix on normal
void ApplyNMatrix(float x, float y, float z, float *out_x, float *out_y, float *out_z);
                
#endif // _MATRIX_HPP

// end of file ----------------------------------------------------------------+ 
