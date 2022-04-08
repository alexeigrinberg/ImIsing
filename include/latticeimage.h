#ifndef LATTICEIMAGE_H
#define LATTICEIMAGE_H

#include <iostream>
#include <iomanip>

class LatticeImage
{
    private:
        int width;
        int height;
        int nchan=4;
    public:
        unsigned char* data;
        //unsigned int color_a = 0xFFFFFFFF;
        //unsigned int color_b = 0x000000FF;

        float* color_a;
        float* color_b;
        
        LatticeImage(int w, int h);
        LatticeImage(int w, int h, int n);
        LatticeImage(int w, int h, float* ca, float* cb);

        int GetWidth() { return width; }
        int GetHeight() { return height; }
        int GetNumChan() { return nchan; }

        bool PrintData();
};

#endif
