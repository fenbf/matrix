// thematrix.cpp

#include "thematrix.hpp"

// global variables -----------------------------------------------------------+
time_t t;
struct tm *tc;

FONT *matrix_font;
int font_height;
int font_width;

// LLine class implementation -------------------------------------------------+
int LLine::line_len = 0;
int LLine::line_col = 0;
float LLine::line_asp = 0.0f;

// Update ---------------------------------------------------------------------+
void LLine::Update(int max)
{
 y+=v;
 if (y > max-1+line_len) Init(1); 
}

// Draw -----------------------------------------------------------------------+
void LLine::Draw(BITMAP *map, int i)
{
 static int p;
 
 for (p = 0; p < line_len; p++)
 {
  if ((int)y-p < 0) break;
  if ((int)y-p > map->h-1) continue;    
  if (p == 0) map->line[(int)y-p][i] = 230; // white-green...
  else                   
  map->line[(int)y-p][i] = line_col-(int)((float)p*line_asp);
 }
} 

// Init -----------------------------------------------------------------------+
void LLine::Init(int max)
{
 y = (float)(-rand()%max);
 v = (float)(rand()%901+100)/333.0f;
}

// function implementations ---------------------------------------------------+

/*-----------------------------------------------------------------------------+
|                               BlitMap                                        |
+-----------------------------------------------------------------------------*/
void BlitMap(BITMAP *bmp, BITMAP *map, int fen_mode)
{
 static int i, j;
 static char buf[2];
 static const char fen[5] = "+FEN";
 static int glyph_random_counter = rand();
 static int randomize_counter = 0;
 
 buf[1] = '\0';
 
 if (fen_mode == TRUE)
 {
  for (i = 0; i < map->w; i++)
  {
   for (j = 0; j < map->h; j++)
   {
   	buf[0] = fen[i%4];
    textout_ex(bmp, matrix_font, buf, i*font_width, j*font_height, map->line[j][i], 0);
   }
  }
 }
 else
 {
  for (i = 0; i < map->w; i++)
  {
   for (j = 0; j < map->h; j++)
   {
    buf[0] = 32+(i*j+j+i+(map->line[j][i] > 60 ? map->line[j][i] : 0))%96;//32+rand()%96;
    textout_ex(bmp, matrix_font, buf, i*font_width, j*font_height, map->line[j][i], 0);
   }
  }
 }
} 

/*-----------------------------------------------------------------------------+
|                               PutLetter                                      |
+-----------------------------------------------------------------------------*/ 
void PutLetter(BITMAP *bmp, int x, int y, int c)
{
 if (x >= 0 && x < bmp->w && y >= 0 && y < bmp->h)
 {
  if (/*bmp->line[y][x] < c &&*/ bmp->line[y][x] > 0) 
   bmp->line[y][x] = c;
 }  
} 

/*-----------------------------------------------------------------------------+
|                               DrawClock                                      |
+-----------------------------------------------------------------------------*/ 
void DrawClock(BITMAP *bmp, float r)
{
 static const float as = 256.0f/60.0f;
 static const float ah = 256.0f/12.0f;

 time(&t);
 tc = localtime(&t);
 
 if (tc->tm_hour > 12) tc->tm_hour-=12;
 
 //text_mode(-1);
 //textprintf_centre(bmp, font, bmp->w>>1, bmp->h-10, color, "%2d:%2d:%2d", tc->tm_hour, tc->tm_min, tc->tm_sec);
 //textout_centre(bmp, font, "The Matrix", bmp-w>>1, (bmp->h>>1)-4, color);
 //text_mode(0);

 // second pointer:
 PushMatrix();
  RotateMatrixY(-as*(float)tc->tm_sec);
  TranslateMatrix(0.0f, 1.0f, 7.0f);
  WireBox(bmp, 1.0f, 1.0f, 14.0f);
 PopMatrix(); 
 
 // minute pointer:
 PushMatrix();
  RotateMatrixY(-as*(float)tc->tm_min);
  TranslateMatrix(0.0f, 0.0f, 5.0f);
  WireBox(bmp, 1.0f, 1.0f, 10.0f);
 PopMatrix(); 
 
 // hour pointer:
 PushMatrix();
  RotateMatrixY(-ah*(float)tc->tm_hour);
  TranslateMatrix(0.0f, -1.0f, 3.0f);
  WireBox(bmp, 1.0f, 1.0f, 6.0f);
 PopMatrix(); 
 
 // big cylinder around pointers                  
 WireCylinder(bmp, 16.0f, 8.0f, 12, 4);
 
 // main axis with wheel
 PushMatrix();
  RotateMatrixY(r);
  TranslateMatrix(0.0f, -24.0f, 0.0f);
  WireCylinder(bmp, 3.0f, 44.0f, 3, 6);
  TranslateMatrix(0.0f, 8.0f, 0.0f);
  WireCylinder(bmp, 10.0f, 4.0f, 8, 1);
 PopMatrix();
 
 // perpedicular axis and wheel
 PushMatrix();
  TranslateMatrix(12.0f, -27.0f, 0.0f);
  RotateMatrixZ(64.0f);
  RotateMatrixY(-r); 
  WireCylinder(bmp, 10.0f, 4.0f, 8, 1);
  TranslateMatrix(0.0f, 16.0f, 0.0f);
  WireCylinder(bmp, 2.0f, 32.0f, 3, 4);
 PopMatrix(); 
}    
                         
// end of file ----------------------------------------------------------------+
