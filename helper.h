#ifndef HELPER_H
#define HELPER_H

#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "ising.h"

char* GenerateRandomData(int width, int height);
unsigned char* SetupImageData(int width, int height, int nchan);
bool UpdateImageFromSim(Ising* sim, unsigned char* image_data, int out_width, int out_height);
bool UpdateImageFromSimRedOnly(Ising* sim, unsigned char* image_data, int out_width, int out_height);
bool SetupTexture(GLuint* out_texture);
bool UpdateTexture(unsigned char* image_data, int out_width, int out_height);
bool PrintData(unsigned char* data, int width, int height, int nchan);
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
#endif /* HELPER_H */

