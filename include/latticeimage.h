#ifndef LATTICEIMAGE_H
#define LATTICEIMAGE_H

#include <iostream>
#include <iomanip>
#include "ising.h"

class LatticeImage
{
    private:
        int width;
        int height;
        int nchan=4;
    public:
        unsigned char* data;
        unsigned int color_a = 0xabcdefFF;
        unsigned int color_b = 0x000000FF;
        
        LatticeImage(int w, int h);
        LatticeImage(int w, int h, int n);

        int GetWidth() { return width; }
        int GetHeight() { return height; }
        int GetNumChan() { return nchan; }

        bool UpdateDataFromSim(Ising* sim);
        bool PrintData();
};

#endif
