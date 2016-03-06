// wire3d.cpp

#include "wire3d.hpp"

// global variables -----------------------------------------------------------+
// extern:
void (*draw_pixel)(BITMAP *, int, int, int);
int color;
int color_min;
float falloff;

// other:
//static int i, j, vc;
static float s, t;

V3D_f *v[96], *vout[96], *vtmp[96];
V3D_f _vout[96], _vtmp[96];
int buf[8];

// helper function prototypes -------------------------------------------------+
int GetColor(float x, float y, float z);
int Clip3D(V3D_f *vtx, int n);
int Process3D(BITMAP *bmp, Vector *verts, int n);
void WireQuad(BITMAP *bmp, Vector &a, Vector &b, Vector &c, Vector &d);

// function implementation ----------------------------------------------------+

// helper functions:

// ----------------------------------------------------------------------------+
// if distance between camera pos and this point (x, y, z) is "falloff" than
// returned color is 0, if the distance is smaller color is bigger
int GetColor(float x, float y, float z)
{
 static float cx, cy, cz, l;
 static int c;
 
 GetCameraPos(cx, cy, cz);
 
 cx = x-cx;
 cy = y-cy;
 cz = z-cz;

 l = sqrt(cx*cx+cy*cy+cz*cz);
 
 if (l > falloff) return 0;
 
 l = (falloff-l)/falloff;
 
 l = MID(0.0f, l, 1.0f);
 
 c = (int)((float)color*l);
 return ( c < color_min ? color_min : c );
}
 
// ----------------------------------------------------------------------------+ 
int Clip3D(V3D_f *vtx, int n)
{
 for (int i = 0; i < n; i++)
  v[i] = &vtx[i];
  
 for (int i = 0; i < n*12; i++)
 {
  vout[i] = &_vout[i];    // vout[i] is pointer so it can't contain any value, only adress !
  vtmp[i] = &_vtmp[i];
 }   
 
 return clip3d_f(POLYTYPE_FLAT, 0.1f, 0.0f, n, (AL_CONST V3D_f **)v, vout, vtmp, buf);
}    

// ----------------------------------------------------------------------------+
// draws wire polygon...
int Process3D(BITMAP *bmp, Vector *verts, int n)
{
 static V3D_f vtx[8];
 static int c[8];

 if (n > 8) return 0;

 for (int i = 0; i < n; i++)
 {
  vtx[i].x = verts[i].x;
  vtx[i].y = verts[i].y;
  vtx[i].z = verts[i].z;
 }
 
 int vc = Clip3D((V3D_f *)vtx, n);
 if (vc < 2) return 0;
 
 // calc color and 2D position:
 for (int i = 0; i < vc; i++)
 {
  //c[i] = GetColor(vout[i]->x, vout[i]->y, vout[i]->z);
  c[i] = color;
  persp_project_f(vout[i]->x, vout[i]->y, vout[i]->z, &vout[i]->x, &vout[i]->y);
 } 
  
 for (int i = 0; i < vc-1; i++)
  do_line(bmp, (int)vout[i]->x, (int)vout[i]->y, (int)vout[i+1]->x, (int)vout[i+1]->y, (c[i]+c[i+1])>>1, draw_pixel); 
 do_line(bmp, (int)vout[vc-1]->x, (int)vout[vc-1]->y, (int)vout[0]->x, (int)vout[0]->y, (c[vc-1]+c[0])>>1, draw_pixel);  
} 

// ----------------------------------------------------------------------------+
void WireQuad(BITMAP *bmp, Vector &a, Vector &b, Vector &c, Vector &d)
{
 Vector out[4] = { a, b, c, d };
 
 Process3D(bmp, (Vector *)out, 4);
}  

/*-----------------------------------------------------------------------------+
|                                 WireBox                                      |
+-----------------------------------------------------------------------------*/
void WireBox(BITMAP *bmp, float sx, float sy, float sz)
{
 sx *= 0.5f;
 sy *= 0.5f;
 sz *= 0.5f;
 
 Vector verts[8];
 verts[0] = Vector(-sx, sy, -sz);  // front face
 verts[1] = Vector(-sx, -sy, -sz);
 verts[2] = Vector(sx, -sy, -sz);
 verts[3] = Vector(sx, sy, -sz);
 verts[4] = Vector(-sx, sy, sz);   // back face
 verts[5] = Vector(-sx, -sy, sz);
 verts[6] = Vector(sx, -sy, sz);
 verts[7] = Vector(sx, sy, sz);
 
 for (int i = 0; i < 8; i++)
  ApplyMatrix(verts[i].x, verts[i].y, verts[i].z, &verts[i].x, &verts[i].y, &verts[i].z);
 
 // draw faces:
 WireQuad(bmp, verts[0], verts[1], verts[2], verts[3]); // front
 WireQuad(bmp, verts[3], verts[2], verts[6], verts[7]); // left
 WireQuad(bmp, verts[7], verts[6], verts[5], verts[4]); // back
 WireQuad(bmp, verts[4], verts[5], verts[1], verts[0]); // right
 WireQuad(bmp, verts[4], verts[0], verts[3], verts[7]); // top
 WireQuad(bmp, verts[5], verts[6], verts[2], verts[1]); // bottom 
}

/*-----------------------------------------------------------------------------+
|                                 WirePlane                                    |
+-----------------------------------------------------------------------------*/
void WirePlane(BITMAP *bmp, float size, int w, int h)
{
 static int  m, n;
 Vector **verts;
 
 // allocate memory for points:
 verts = new Vector*[w+1];
 for (int i = 0; i < w+1; i++)
  verts[i] = new Vector[h+1]; 
  
 s = -size*(float)w*0.5f; 
 t = -size*(float)h*0.5f;
 
 // initialize them:
 for (m = 0; m < w+1; m++)
 {
  for (n = 0; n < h+1; n++)
  {
   verts[m][n] = Vector(s+(float)m*size, 0.0f, t+(float)n*size);
   ApplyMatrix(verts[m][n].x, verts[m][n].y, verts[m][n].z, &verts[m][n].x, &verts[m][n].y, &verts[m][n].z);
  }
 }
 
 // draw them:
 for (m = 0; m < w; m++)
 {
  for (n = 0; n < h; n++)    
    WireQuad(bmp, verts[m][n], verts[m][n+1], verts[m+1][n+1], verts[m+1][n]);
 } 
 
 // clean up:
 for (int i = 0; i < w+1; i++)
  delete [] verts[i];
 delete [] verts; 
}                       

/*-----------------------------------------------------------------------------+
|                                WireCylinder                                  |
+-----------------------------------------------------------------------------*/
void WireCylinder(BITMAP *bmp, float r, float h, int faces, int parts, int top, int bottom)
{
 static float ang, b, x, z;
 static Vector **verts;
 static int m, n;

 s = h*0.5f;
 const float angDelta = (M_PI*2.0f)/(float)faces;
 b = h/(float)parts;
 
 verts = new Vector*[parts+1];
 for (n = 0; n < parts+1; n++)                
  verts[n] = new Vector[faces];
 
 // get points:
 ang = 0.0f;
 for (n = 0; n < faces; n++)
 {
  x = cosf(ang)*r;
  z = sinf(ang)*r;
  
  for (m = 0; m < parts+1; m++)
  {
   verts[m][n] = Vector(x, -s+b*(float)m, z);
   ApplyMatrix(verts[m][n].x, verts[m][n].y, verts[m][n].z, &verts[m][n].x, &verts[m][n].y, &verts[m][n].z);
  } 
  ang+=angDelta;  
 } 
 
 for (n = 0; n < parts; n++)
 {
  for (m = 0; m < faces-1; m++)
   WireQuad(bmp, verts[n][m], verts[n+1][m], verts[n+1][m+1], verts[n][m+1]);
  WireQuad(bmp, verts[n][m], verts[n+1][m], verts[n+1][0], verts[n][0]);
 }

 for (m = 0; m < parts+1; m++)
  delete [] verts[m];
 delete [] verts; 
} 
 
// end of file ----------------------------------------------------------------+ 
