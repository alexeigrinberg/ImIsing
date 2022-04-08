#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "helper.h"
#include <iostream>
#include <iomanip>

char* GenerateRandomData(int width, int height)
{
    char* data = new char[width*height];
    for (int i=0; i<width*height; i++)
        data[i] = std::rand() % 2;
    return data;
}

unsigned char* SetupImageData(int width, int height, int nchan)
{
    unsigned char* data = new unsigned char[width*height*nchan];
    for (int i=0; i<width*height*nchan; i++)
        data[i] = 255;
    return data;
}


bool UpdateImageFromSim(Ising* sim, unsigned char* image_data, int out_width, int out_height, unsigned int color_a, unsigned int color_b)
{
    if (sim->data == NULL || image_data == NULL)
        return false;

    int in_width = sim->GetWidth();
    int in_height = sim->GetHeight();

    int scale_x = out_width / in_width;
    int scale_y = out_height / in_height;

    for (int i=0; i<in_height; i++){
        for (int j=0; j<in_width; j++){
            // upscale each pixel in sim data
            char val = sim->data[i*in_width+j];
            
            for (int x=0; x<scale_y; x++){
                for (int y=0; y<scale_x; y++){
                    
                    int idx = out_width*(scale_y*i+x)+scale_x*j+y;
                    if (val==1)
                    {
                        image_data[4*idx] = (color_a >> 24) & 0xFF;
                        image_data[4*idx+1] = (color_a >> 16) & 0xFF;
                        image_data[4*idx+2] = (color_a >> 8) & 0xFF;
                        image_data[4*idx+3] = color_a & 0xFF;
                    }
                    else
                    {
                        image_data[4*idx] = (color_b >> 24) & 0xFF;
                        image_data[4*idx+1] = (color_b >> 16) & 0xFF;
                        image_data[4*idx+2] = (color_b >> 8) & 0xFF;
                        image_data[4*idx+3] = color_b & 0xFF;
                    }                
                }
            }
        }
    }
    return true;
}

bool UpdateImageFromSim(Ising* sim, LatticeImage* image)
{
    if (sim->data == NULL || image->data == NULL)
        return false;

    int in_width = sim->GetWidth();
    int in_height = sim->GetHeight();
    
    int out_width = image->GetWidth();
    int out_height = image->GetHeight();

    int scale_x = out_width / in_width;
    int scale_y = out_height / in_height;
    
    int nchan = image->GetNumChan();
    int color_up = image->color_a;
    int color_down = image->color_b;

    for (int i=0; i<in_height; i++)
    {
        for (int j=0; j<in_width; j++)
        {
            // upscale each pixel in sim data
            char val = sim->data[i*in_width+j];
            
            for (int x=0; x<scale_y; x++)
            {
                for (int y=0; y<scale_x; y++)
                {
                    int idx = out_width*(scale_y*i+x)+scale_x*j+y;
                    
                    // loop over each color channel
                    for (int n=0; n<nchan; n++)
                    {
                        if (val==1)
                            image->data[nchan*idx+n] = (image->color_a >> 8*(nchan-1-n)) & 0xFF;
                        else
                            image->data[nchan*idx+n] = (image->color_b >> 8*(nchan-1-n)) & 0xFF;
                    }

                }
            }
        }
    }
    return true;
}

bool UpdateImageFromSimRedOnly(Ising* sim, unsigned char* image_data, int out_width, int out_height)
{
    if (sim->data == NULL || image_data == NULL)
        return false;
    
    int in_width = sim->GetWidth();
    int in_height = sim->GetHeight();
    
    int scale_x = out_width / in_width;
    int scale_y = out_height / in_height;

    for (int i=0; i<in_height; i++){
        for (int j=0; j<in_width; j++){
            // upscale each pixel in sim data
            char val = sim->data[i*in_width+j];
            
            for (int x=0; x<scale_y; x++){
                for (int y=0; y<scale_x; y++){
                    int idx = out_width*(scale_y*i+x)+scale_x*j+y;
                    
                    if (val==1)
                        image_data[idx] = 255;
                    else
                        image_data[idx] = 0;
                }
            }
        }
    }
    return true;
}

bool SetupTexture(GLuint* out_texture)
{
    // Create an OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR for byte / GL_NEAREST for int
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_LINEAR for byte / GL_NEAREST for int
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
    
    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    *out_texture = image_texture;
    
    return true;
}

bool UpdateTexture(unsigned char* image_data, int out_width, int out_height)
{
    if (image_data == NULL)
        return false;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out_width, out_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    return true;
}

bool PrintData(unsigned char* data, int width, int height, int nchan)
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

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;
    
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}
