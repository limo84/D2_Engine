#pragma once

#include <stdbool.h>

#include "d2_structs.h"

typedef struct {
  float *projection; // converts clipping space to screen coords
  float *view; // "moves camera" or rather all objects (to l if cam moves r)
  float *vp;
  float *model;
  float *scale;
  float *translate;
  float *rotate;
} Matrices;

// SDL_Window *_Engine_CreateSdlWindow(Uint32 width, Uint32 height, bool
// fullscreen);
// GLFWwindow *_Engine_CreateGlfwWindow(u32 width, u32 height, bool fullscreen);
// SDL_Window *_Engine_CreateSdlWindow(u32 width, u32 height, bool fullscreen);

void _Backend_CreateWindow(u32 width, u32 height, bool fullscreen);

void _Engine_InitMatrices();
void _Engine_InitUniformLocs();
void _Engine_UpdateProjectionMatrix();
void _Engine_CreateQuadVAO();
void _Engine_CreatePolyVAO();
void _Backend_SwapBuffers();
void _Backend_CalculateDelta();
void _Backend_InitTime();

void _Backend_HandleEvents();
