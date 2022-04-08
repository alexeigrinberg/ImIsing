#ifndef HELPER_H
#define HELPER_H

#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "ising.h"
#include "latticeimage.h"


bool UpdateImageFromSim(Ising* sim, LatticeImage* image);
bool SetupTexture(GLuint* out_texture);
bool UpdateTexture(LatticeImage* image);
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
#endif /* HELPER_H */

