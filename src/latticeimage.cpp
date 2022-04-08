#include "latticeimage.h"

LatticeImage::LatticeImage(int w, int h)
{
    width = w;
    height = h;
    data = new unsigned char[w*h*nchan]; 
}

LatticeImage::LatticeImage(int w, int h, int n)
{
    width = w;
    height = h;
    nchan = n;
    data = new unsigned char[w*h*n];
}

bool LatticeImage::UpdateDataFromSim(Ising* sim)
{
    if (sim->data == NULL || data == NULL)
        return false;

    int in_width = sim->GetWidth();
    int in_height = sim->GetHeight();
    
    int scale_x = width / in_width;
    int scale_y = height / in_height;
    
    for (int i=0; i<in_height; i++){
        for (int j=0; j<in_width; j++){
            // upscale each pixel in sim data
            char val = sim->data[i*in_width+j];
            
            for (int x=0; x<scale_y; x++){
                for (int y=0; y<scale_x; y++){
                    int idx = width*(scale_y*i+x)+scale_x*j+y;
                    
                    // loop over each color channel
                    for (int n=0; n<nchan; n++){
                        if (val==1)
                            data[nchan*idx+n] = (color_a >> 8*(nchan-1-n)) & 0xFF;
                        else
                            data[nchan*idx+n] = (color_b >> 8*(nchan-1-n)) & 0xFF;
                    }

                }
            }
        }
    }
    return true;
}

bool LatticeImage::PrintData()
{
    if (data == NULL)
        return false;
    for (int i=0; i<height; i++){
        for (int j=0; j<width*nchan; j++){
            std::cout << std::setw(3) << (int) data[i*width*nchan + j] << ' ';
        }
        std::cout << std::endl;
    }
    return true;
}
