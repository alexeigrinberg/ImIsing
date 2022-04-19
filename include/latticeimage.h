#ifndef LATTICEIMAGE_H
#define LATTICEIMAGE_H

#include <iostream>
#include <iomanip>

class LatticeImage
{
    private:
        int width;
        int height;
    public:
        unsigned char* data;

        float* color_a;
        float* color_b;
        
        LatticeImage(int w, int h);
        LatticeImage(int w, int h, float* ca, float* cb);

        int GetWidth() { return width; }
        int GetHeight() { return height; }

        bool PrintData();
};

#endif
