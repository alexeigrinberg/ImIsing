#include "latticeimage.h"

LatticeImage::LatticeImage(int w, int h)
{
    width = w;
    height = h;
    data = new unsigned char[w*h*4];
    color_a = new float[4];
    color_b = new float[4];
    
    color_a[0] = 0.212f;
    color_a[1] = 0.285f;
    color_a[2] = 0.833f;
    color_a[3] = 1.0f;

    color_b[0] = 0.0f;
    color_b[1] = 0.0f;
    color_b[2] = 0.0f;
    color_b[3] = 1.0f;
}


LatticeImage::LatticeImage(int w, int h, float* ca, float* cb)
{
    width = w;
    height = h;
    data = new unsigned char[w*h*4];
    color_a = ca;
    color_b = cb;
}

bool LatticeImage::PrintData()
{
    if (data == NULL)
        return false;
    for (int i=0; i<height; i++){
        for (int j=0; j<width*4; j++){
            std::cout << std::setw(3) << (int) data[i*width*4 + j] << ' ';
        }
        std::cout << std::endl;
    }
    return true;
}
