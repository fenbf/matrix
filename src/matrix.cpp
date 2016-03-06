// matrix.cpp
// Warning! Code is not good! It is sufficient for small transformation...
//
// 20 June 2003
// Fen

#include "matrix.hpp" 

// matrix stack ---------------------------------------------------------------+
// I do not use list to implement Stack, it is only a array. With this
// code is simple. Structures are not big, so the memory is not
// burden with data
//
// size: ( 64 + 12 ) * MAX_MSTACK_DEPTH [bytes]
//
// Please note that scaling is not added to "matrices", it is done separately.
// With this rest of transformation are very simple to implement...
//
// there is only one camera matrix, but it not needs to add camera to stack.
// We don't need functions like PushCameraMatrix... it would be useless...

MATRIX_f matrices[MAX_MSTACK_DEPTH];          // points transformations
MATRIX_f camera_m;                            // camera matrix
float cam_x, cam_y, cam_z;                    // camera pos
float scale_x[MAX_MSTACK_DEPTH];              // scaling...
float scale_y[MAX_MSTACK_DEPTH];
float scale_z[MAX_MSTACK_DEPTH];
int current;                                  // current index in stack...

// variables for internal usage -----------------------------------------------+
MATRIX_f buf1, buf2, rot_m[4];
float mx, my, mz, cx, cy, cz;                  // buffer for translation
float fx[3], fy[3], fz[3], upx, upy, upz;

// function implementations ---------------------------------------------------+

// managment ------------------------------------------------------------------+

// Initializes matrix stack. Call it first! 
void InitMatrices()
{
 // clear all structures to "do nothing":
 for (register int i = 0; i < MAX_MSTACK_DEPTH; i++)
 {
  matrices[i] = identity_matrix_f;
  scale_x[i] = scale_y[i] = scale_z[i] = 1.0f;
 } 
 
 camera_m = identity_matrix_f;
 
 current = 0;
}

// Resets current matrix to identity matrix
void ResetMatrix()
{
 matrices[current] = identity_matrix_f;
 scale_x[current] = scale_y[current] = scale_z[current] = 1.0f;
}

// Pushes current matrix to the top of the stack
int PushMatrix()
{
 if (current+1 < MAX_MSTACK_DEPTH)
 {
  current++;
  // new matrix is same as befor:
  matrices[current] = matrices[current-1];
  scale_x[current] = scale_x[current-1];
  scale_y[current] = scale_y[current-1];
  scale_z[current] = scale_z[current-1];
 }   
 else
  return 0;
  
 return 1;
}

// Pops matrix from the stack
int PopMatrix()
{
 if (current > 0)
 {
  // clear current matrix to 0
 // matrices[current] = identity_matrix_f;
 // scale[current] = XYZ(1.0f);
  current--;      // move to matrix thas is before
 } 
 else
  return 0;
  
 return 1;
}

// Transformations ------------------------------------------------------------+

// Muls two matrices - for internal usage
inline void Mul()
{
 buf2 = matrices[current];
// matrices[current] = identity_matrix_f;
 matrix_mul_f(&buf2, &buf1, &matrices[current]); 
} 

// helper macros --------------------------------------------------------------+

// copies translation to "mx, my and mz" and set translation to 0:
#define CLEAR_TRANSLATION()                                      \
                            mx = matrices[current].t[0];         \
                            my = matrices[current].t[1];         \
                            mz = matrices[current].t[2];         \
                                                                 \
                            matrices[current].t[0] = 0.0f;       \
                            matrices[current].t[1] = 0.0f;       \
                            matrices[current].t[2] = 0.0f;       

// restore translation from "mx, my and mz":
#define RESTORE_TRANSLATION()                                    \
                              matrices[current].t[0] = mx;       \
                              matrices[current].t[1] = my;       \
                              matrices[current].t[2] = mz;
                              
// ----------------------------------------------------------------------------+ 
void TranslateMatrix(float x, float y, float z)
{
 // first rotate coordinate system and
 // than translate it in appropriate direction:
 
 CLEAR_TRANSLATION();
 
 apply_matrix_f(&matrices[current], -x, y, z, &x, &y, &z);
 
 matrices[current].t[0] = (x*scale_x[current])+mx;
 matrices[current].t[1] = (y*scale_y[current])+my;
 matrices[current].t[2] = (z*scale_z[current])+mz;
} 

// ----------------------------------------------------------------------------+
// Note: rotation is made clockwise in accordance with positive turn of axis.
// Rotations are wery bad. Before rotation coordinate sustem must be
// rotated, but it isn't. I can't find math algotithms... ;(                            
void RotateMatrix(float ax, float ay, float az)
{
 CLEAR_TRANSLATION();

/* apply_matrix_f(&matrices[current], 1.0f, 0.0f, 0.0f, &fx[0], &fy[0], &fz[0]);
 apply_matrix_f(&matrices[current], 0.0f, 1.0f, 0.0f, &fx[1], &fy[1], &fz[1]);
 apply_matrix_f(&matrices[current], 0.0f, 0.0f, 1.0f, &fx[2], &fy[2], &fz[2]);

 get_vector_rotation_matrix_f(&rot_m[0], fx[0], fy[0], fz[0], ax);
 get_vector_rotation_matrix_f(&rot_m[1], fx[1], fy[1], fz[1], -ay);
 get_vector_rotation_matrix_f(&rot_m[2], fx[2], fy[2], fz[2], -az);

 matrix_mul_f(&rot_m[1], &rot_m[2], &rot_m[3]);
 matrix_mul_f(&rot_m[3], &rot_m[0], &buf1); */

 get_rotation_matrix_f(&buf1, ax, -ay, -az);
 Mul();
 
 RESTORE_TRANSLATION();
}

// ----------------------------------------------------------------------------+ 
// the sheme is like obove: first remove translation and do rotation. After
// this restore translation. But in this three functions we don't need
// to use CLEAR OR RESTORE _TRANSLATION macros. We do it ourselves
void RotateMatrixX(float ax)
{
 CLEAR_TRANSLATION();

 apply_matrix_f(&matrices[current], 1.0f, 0.0f, 0.0f, &fx[1], &fy[1], &fz[1]);
 apply_matrix_f(&matrices[current], mx, my, mz, &fx[2], &fy[2], &fz[2]);
 get_vector_rotation_matrix_f(&buf1, fx[1], fy[1], fz[1], -ax);
 qtranslate_matrix_f(&buf1, fx[2], fy[2], fz[2]);
 Mul();
 
 RESTORE_TRANSLATION();
}

// ----------------------------------------------------------------------------+ 
void RotateMatrixY(float ay)
{
 CLEAR_TRANSLATION();

 apply_matrix_f(&matrices[current], 0.0f, 1.0f, 0.0f, &fx[1], &fy[1], &fz[1]);
 apply_matrix_f(&matrices[current], mx, my, mz, &fx[2], &fy[2], &fz[2]);
 get_vector_rotation_matrix_f(&buf1, fx[1], fy[1], fz[1], -ay);
 qtranslate_matrix_f(&buf1, fx[2], fy[2], fz[2]);
 Mul();
 
 RESTORE_TRANSLATION();
}

// ----------------------------------------------------------------------------+
void RotateMatrixZ(float az)
{
 CLEAR_TRANSLATION();

 apply_matrix_f(&matrices[current], 0.0f, 0.0f, 1.0f, &fx[1], &fy[1], &fz[1]);
 apply_matrix_f(&matrices[current], mx, my, mz, &fx[2], &fy[2], &fz[2]);
 get_vector_rotation_matrix_f(&buf1, fx[1], fy[1], fz[1], -az);
 qtranslate_matrix_f(&buf1, fx[2], fy[2], fz[2]);
 Mul();
 
 RESTORE_TRANSLATION();
}

// ----------------------------------------------------------------------------+        
void ScaleMatrix(float sx, float sy, float sz)
{
 scale_x[current] *= sx;
 scale_y[current] *= sy;
 scale_z[current] *= sz;
}

// ----------------------------------------------------------------------------+
// Camera:
void CameraMatrix(float x, float y, float z,
            float xfront, float yfront, float zfront,
            float xup, float yup, float zup, float fov, float asp)
{
 cam_x = x;
 cam_y = y;
 cam_z = z;
 
 get_camera_matrix_f(&camera_m, -x, y, z, -xfront, yfront, zfront, xup, yup, zup, fov, asp);
}            

void CameraLookAt(float x, float y, float z,
                  float cx, float cy, float cz, float fov, float asp)
{
 float vx, vy, vz, xup, yup, zup;
 MATRIX_f roll;
 
 vx = cx-x;
 vy = cy-y;
 vz = cz-z;
 
 cam_x = x;
 cam_y = y;
 cam_z = z;
 
 get_vector_rotation_matrix_f(&roll, -vx, vy, vz, 128.0f);
 apply_matrix_f(&roll, 0.0f, -1.0f, 0.0f, &xup, &yup, &zup); 
 
 get_camera_matrix_f(&camera_m, -x, y, z, -vx, vy, vz, xup, yup, zup, fov, asp);
}

void GetCameraPos(float &x, float &y, float &z)
{
 x = cam_x;
 y = cam_y;
 z = cam_z;
} 
 
// Applies current matrix & camera on the given vertex ------------------------+
// After that transformation vertex is ready to be translated into 2D space.
void ApplyMatrix(float x, float y, float z, float *out_x, float *out_y, float *out_z)
{  
 // first do the scaling...
 apply_matrix_f(&matrices[current], -x*scale_x[current],
                                    y*scale_y[current],
                                    z*scale_z[current],
                                    out_x, out_y, out_z);

 // camera:
 apply_matrix_f(&camera_m, *out_x, *out_y, *out_z, out_x, out_y, out_z);
} 

// Applies current matrix on given normal -------------------------------------+
// Normals are only rotated, if they were scaled or translated lighting
// would be very ugly, or even wouldn't be visible...
void ApplyNMatrix(float x, float y, float z, float *out_x, float *out_y, float *out_z)
{
 CLEAR_TRANSLATION();
 
 // translation from camera matrix:
 cx = camera_m.t[0];
 cy = camera_m.t[1];
 cz = camera_m.t[2];
 
 camera_m.t[0] = 0.0f;
 camera_m.t[1] = 0.0f;
 camera_m.t[2] = 0.0f;
 
 apply_matrix_f(&matrices[current], -x, y, z, out_x, out_y, out_z);
 apply_matrix_f(&camera_m, *out_x, *out_y, *out_z, out_x, out_y, out_z);
 
 // restore translation from camera matrix
 camera_m.t[0] = cx;
 camera_m.t[1] = cy;
 camera_m.t[2] = cz;
 
 RESTORE_TRANSLATION();
} 
             
// end of file ----------------------------------------------------------------+   
