#include <allegro.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "wire3d.hpp"
#include "thematrix.hpp"

// program info ---------------------------------------------------------------+
#define PROG_NAME "The Matrix"
#define PROG_AUTHOR "Bart \"Fen\" Filipek"

// some defines ---------------------------------------------------------------+
#define SETUP_3D

#define DEF_G GFX_AUTODETECT_WINDOWED
#define DEF_W 640
#define DEF_H 480
#define DEF_B 8

#define ANIMATION_SPEED 25

// timer ----------------------------------------------------------------------+
volatile int fps = 0;
volatile int frame = 0;
volatile int anim = 0;
volatile int pauseCamera = 0;

void UpdateFps()
{
 fps = frame;
 frame = 0;
}
END_OF_FUNCTION(UpdateFps); 

void UpdateAnim()
{
 anim++;
}
END_OF_FUNCTION(UpdateAnim);

// global variables -----------------------------------------------------------+
PALETTE pal;
int scr_w, scr_h, bpp;

// double buffering:
BITMAP *buffer;

// matrix bitmap:
BITMAP *map;

// switches:
int pause;
int info_on;
int fen_mode;

// Lines:
LLine *lines;
Vector rot;

float r;

// function prototypes --------------------------------------------------------+
void TheMatrix();
int SetupGFX();
void SetPalette();

int Init();
void Deinit();
void Setup3D();
int ProcessInput();
void DrawScene();
void UpdateAnimation();

/*-----------------------------------------------------------------------------+
|                         BEGIN OF THE MAIN                                    |
+-----------------------------------------------------------------------------*/
int main()
{
 if (allegro_init() != 0)
  return 0;
  
 install_timer();
 install_mouse();
 install_keyboard();
 
 set_window_title(PROG_NAME);
 
 if (SetupGFX() == FALSE) return 1;
 
 if (Init() == FALSE) return 1; 
 
 // call for main function:
 TheMatrix();
 
 Deinit();
 
 return 0;
}

END_OF_MAIN();
/*-----------------------------------------------------------------------------+
|                          END OF THE MAIN                                     |
+-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------+
|                                 TheMatrix                                    |
+-----------------------------------------------------------------------------*/
void TheMatrix()
{  
 // timer - fps counter
 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame);
 LOCK_VARIABLE(anim);
 LOCK_FUNCTION(UpdateFps);
 LOCK_FUNCTION(UpdateAnim);
 install_int(UpdateFps, 1000);
 install_int_ex(UpdateAnim, BPS_TO_TIMER(ANIMATION_SPEED));
 
 for ( ;; )
 {
  if (ProcessInput() == FALSE) break;
  
  while (anim > 0)
  {   
   if (pause == FALSE)
    UpdateAnimation();
   
   anim--;
  }  
  
  DrawScene();                                      
  
  if (info_on == TRUE)
  {
   textprintf_ex(buffer, font, 10, 10, 255, 0, "fps: %d  ", fps);
   textout_ex(buffer, font, "Hide INFO [F1]", 10, 20, 255, 0);
   textout_ex(buffer, font, "Fen Mode [F2]", 10, 30, 255, 0);
   textout_ex(buffer, font, "Pause Camera [F3]", 10, 40, 255, 0);
   textout_ex(buffer, font, "Pause All [Space]", 10, 50, 255, 0);
   textout_ex(buffer, font, "www.bfilipek.com", SCREEN_W-160, SCREEN_H-24, 255, 0);
  }
  
  blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
 vsync();
  
  frame++;
 } 
} 

/*-----------------------------------------------------------------------------+
|                                  SetupGFX                                    |
+-----------------------------------------------------------------------------*/
int SetupGFX()
{
 int gfx_mode;
 
 #ifdef SETUP_3D
  if (set_gfx_mode(GFX_GDI, 320, 200, 0, 0) != 0)
  {
   set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
   allegro_message("Cannot set any GFX mode!");
   allegro_exit();
   return FALSE;
  }

  set_palette(desktop_palette);
 
  clear_to_color(screen, 0);
  gui_fg_color = 255;
  gui_bg_color = 0;
 #endif // SETUP_3D
 
 gfx_mode = DEF_G;
 scr_w = 1600;
 scr_h = 900;
 bpp = DEF_B;
 
 //GFX_MODE_LIST *gfxList = get_gfx_mode_list(GFX AUTODETECT);
 //int count = gfxList->num_modes;
 
 
 #ifdef SETUP_3D
  //if (!gfx_mode_select(&gfx_mode, &scr_w, &scr_h))
  //{
  // allegro_exit();
  // return FALSE;
  //} 
 #endif // SETUP_3D 
 
 set_color_depth(bpp);
 int ret = set_gfx_mode(gfx_mode, scr_w, scr_h, 0, 0);
 if (ret != 0)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Error %s", allegro_error);
  allegro_exit();
  return FALSE;
 }
 
 return TRUE;
}                 

/*-----------------------------------------------------------------------------+
|                                 SetPalette                                   |
+-----------------------------------------------------------------------------*/
void SetPalette()
{
  static int base = 0;

 for (int i = 0; i < 256; i++)
 {
  if (i < 64-base) pal[i].g = i+base;
  else pal[i].g = 63;
  
  if (i > 63-base && i < 127-base<<1) pal[i].r = pal[i].b = (i-(63-base));
  else pal[i].r = pal[i].b = 0;
 }
 
 pal[255].r = pal[255].g = pal[255].b = 63;
 set_palette(pal);
}

/*-----------------------------------------------------------------------------+
|                                   Init                                       |
+-----------------------------------------------------------------------------*/
int Init()
{
 // double buffering:
 buffer = create_bitmap(SCREEN_W, SCREEN_H);
 clear(buffer); 
 
 // create "char map":
 map = create_bitmap(SCREEN_W>>3, SCREEN_H>>3);
 if (map == NULL) return FALSE;
 clear(map);
 
 // lines:
 lines = new LLine[map->w];  
 LLine::line_len = (map->h<<1)+(map->h>>2);
 LLine::line_col = 45;
 LLine::line_asp = (float)LLine::line_col/(float)LLine::line_len;
 for (int i = 0; i < map->w; i++)
 lines[i].Init(map->h<<1);
 
 SetPalette();
 
 set_projection_viewport(0, 0, scr_w/8, scr_h/8);
 InitMatrices();
 
 draw_pixel = PutLetter;
 color = 75;
 color_min  = 0;
 falloff = 150.0f;
 
 pause = FALSE;
 info_on = TRUE;
 fen_mode = FALSE;
 r = 0.0f;
 return TRUE;
} 

/*-----------------------------------------------------------------------------+
|                                   Deinit                                     |
+-----------------------------------------------------------------------------*/
void Deinit()
{
 if (buffer != NULL) destroy_bitmap(buffer);
 if (map != NULL) destroy_bitmap(map);
 
 if (lines != NULL) delete [] lines;
 lines = NULL;
} 

/*-----------------------------------------------------------------------------+
|                               ProcessInput                                   |
+-----------------------------------------------------------------------------*/
int ProcessInput()
{
 static int keys[4] = { 0, 0, 0, 0};
 static int p = 0;
 static char buf[20];
 
 if (key[KEY_ESC]) return FALSE;
  
 // restore default
 if (key[KEY_ENTER])
 {
  rot = Vector();
 } 

 // photo:
 if (key[KEY_HOME])
 {
  sprintf(buf, "scr_shot%d.pcx", p++);
  save_pcx(buf, buffer, NULL);
  rest(100);
 }  
  
 // pause:
 if (key[KEY_SPACE] && keys[0] == 0)
 {
  pause = ( pause == TRUE ? FALSE : TRUE );
  keys[0] = fps>>2;
 } 
  
 // some info in top left corner
 if (key[KEY_F1] && keys[1] == 0)
 {
  info_on = ( info_on == TRUE ? FALSE : TRUE );
  keys[1] = fps>>2;
 }
 
 // "fen" mode
 if (key[KEY_F2] && keys[2] == 0)
 {
  fen_mode = ( fen_mode == TRUE ? FALSE : TRUE );
  keys[2] = fps>>2;
 } 
 
 // "fen" mode
 if (key[KEY_F3] && keys[3] == 0)
 {
  pauseCamera = ( pauseCamera == TRUE ? FALSE : TRUE );
  keys[3] = fps>>2;
 } 
 
 if (keys[0] > 0) keys[0]--;
 if (keys[1] > 0) keys[1]--;
 if (keys[2] > 0) keys[2]--;
 if (keys[3] > 0) keys[3]--;
  
 return TRUE;
}

/*-----------------------------------------------------------------------------+
|                                 DrawScene                                    |
+-----------------------------------------------------------------------------*/
void DrawScene()
{
 static int i;
 static float sx, sy, sz, /*cx,*/ cy/*, cz*/;
 static float cam_x, cam_y, cam_z;
 static float look_x, look_y, look_z;

 if (fen_mode == TRUE)
 {
  color_min =10;
  clear_to_color(map, color_min);
 }
 else
 {
  color_min = 0;  
  clear_to_color(map, 0);
  for (i = 0; i < map->w; i++)
   lines[i].Draw(map, i); 
 }
     
 sx = sin(rot.x);
 sy = sin(rot.y);
 sz = sin(rot.z);
 //cx = cos(rot.x);
 cy = cos(rot.y);
 //cz = cos(rot.z);
 
 cam_x = 36.0f*sx*sy;
 cam_y = 4.0f+24.0f*cy;
 cam_z = 12.0f-36.0f*sz;
 
 look_x = 8.0f*sx*sy;
 look_y = 12.0f*cy;
 look_z = 6.0f-18.0f*sz;      
     
 CameraLookAt(cam_x, cam_y, cam_z, look_x, look_y, look_z, 48.0f, (float)scr_w/(float)scr_h);  
       
 ResetMatrix();
 RotateMatrix(64.0f, 0.0f, 0.0f);
 DrawClock(map, r); 
   
 BlitMap(buffer, map, fen_mode); 
}

/*-----------------------------------------------------------------------------+
|                              UpdateAnimation                                 |
+-----------------------------------------------------------------------------*/
void UpdateAnimation()
{
 static int i;
 //static int a = 0;
 
 if (fen_mode == FALSE)
 {
  for (i = 0; i < map->w; i++)
   lines[i].Update(map->h);
 }
 
 if (pauseCamera == FALSE) 
 {
 rot.x+=0.005f;
 rot.y+=0.012f;
 rot.z+=0.02f;
}
 r-=1.5f;

}   

// end of file ----------------------------------------------------------------+      
