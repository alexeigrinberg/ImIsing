#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "helper.h"

bool UpdateImageFromSim(Ising* sim, LatticeImage* image)
{
    if (sim->data == NULL || image->data == NULL)
        return false;

    int in_width = sim->GetWidth();
    int in_height = sim->GetHeight();
    
    int out_width = image->GetWidth();
    int out_height = image->GetHeight();
    int nchan = image->GetNumChan();
    unsigned char* data = image->data;
    
    int scale_x = out_width / in_width;
    int scale_y = out_height / in_height;
    
    for (int i=0; i<in_height; i++)
    {
        for (int j=0; j<in_width; j++)
        {
            // upscale each pixel in sim data
            char val = sim->data[i*in_width+j];
            
            float*  color = (val==1)?image->color_a:image->color_b;
            //unsigned int color = (val==1)?image->color_a:image->color_b;
            
            for (int x=0; x<scale_y; x++)
            {
                for (int y=0; y<scale_x; y++)
                {
                    int idx = out_width*(scale_y*i+x)+scale_x*j+y;
                    // loop over each color channel
                    switch (nchan)
                    {
                        case 1:
                            data[idx] = (unsigned int)(color[0]*255.0f);
                            
                            //data[idx] = (color >> 24) & 0xFF;
                            
                            break;
                        case 2:
                            data[2*idx] = (unsigned int)(color[0]*255.0f);
                            data[2*idx+1] = (unsigned int)(color[1]*255.0f);

                            //data[2*idx] = (color >> 24) & 0xFF;
                            //data[2*idx+1] = (color >> 16) & 0xFF;
                            
                            break;
                        case 3:
                            data[3*idx] = (unsigned int)(color[0]*255.0f);
                            data[3*idx+1] = (unsigned int)(color[1]*255.0f);
                            data[3*idx+2] = (unsigned int)(color[2]*255.0f);
                            
                            //data[3*idx] = (color >> 24) & 0xFF;
                            //data[3*idx+1] = (color >> 16) & 0xFF;
                            //data[3*idx+2] = (color >> 8) & 0xFF;
                            
                            break;
                        case 4:
                            data[4*idx] = (unsigned int)(color[0]*255.0f);
                            data[4*idx+1] = (unsigned int)(color[1]*255.0f);
                            data[4*idx+2] = (unsigned int)(color[2]*255.0f);
                            data[4*idx+3] = (unsigned int)(color[3]*255.0f);
                            
                            //data[4*idx] = (color >> 24) & 0xFF;
                            //data[4*idx+1] = (color >> 16) & 0xFF;
                            //data[4*idx+2] = (color >> 8) & 0xFF;
                            //data[4*idx+3] = color & 0xFF;
                            
                            break;
                        default:
                            return false;
                    }
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

bool UpdateTexture(LatticeImage* image)
{
    if (image->data == NULL)
        return false;
    switch(image->GetNumChan())
    {
        case 1:
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image->GetWidth(), image->GetHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, image->data);
           break;
        case 2:
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, image->GetWidth(), image->GetHeight(), 0, GL_RG, GL_UNSIGNED_BYTE, image->data);
           break;
        case 3:
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->GetWidth(), image->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
           break;
        case 4:
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
           break;
        default:
           return false;
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
