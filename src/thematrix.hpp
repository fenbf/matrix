// thematrix.hpp

#ifndef THE_MATRIX_HPP
 #define THE_MATRIX_HPP
 
#include <allegro.h>
#include <time.h>
#include "wire3d.hpp"

#define TEST_3D_ONLY

// LLine class ---------------------------------------------------------------+
class LLine
{
 public:
        LLine() { y = 0.0f; v = 0.0f; }
        ~LLine() { }
        
        void Update(int max);
        void Draw(BITMAP *map, int i);
        void Init(int max);
        int GetPos() { return (int)y; }
        
        static int line_len;
        static int line_col;
        static float line_asp;
 private:       
        float y;
        float v;
};

// function prototypes --------------------------------------------------------+
void BlitMap(BITMAP *bmp, BITMAP *map, int fen_mode); 
void PutLetter(BITMAP *bmp, int x, int y, int c);
void DrawClock(BITMAP *bmp, float r);

#endif // THE_MATRIX_HPP

// end of file ----------------------------------------------------------------+ 
