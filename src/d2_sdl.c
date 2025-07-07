#ifndef USE_GLFW

#include "d2.h"
#include "d2_keys.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_mixer.h>

struct Engine {
  SDL_Window *window;
  // GLFWevent event;
  bool isRunning;
};

Engine *engine;

extern bool keys[512];
extern bool oldKeys[512];
extern Mouse mouse;
extern Mouse oldMouse;

double startTime, endTime;
double timeSinceLastFpsPrint = 0;
u16 deltaInMillis = 0;
u64 frames = 0;

bool vsyncEnabled = 1; // TODO
u8 mon = 1;            // TODO

Mix_Music *music;

void _Backend_CreateWindow(u32 width, u32 height, bool fullscreen) {

  engine = malloc(sizeof(Engine));
  engine->isRunning = true;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
    exit(-1);
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    printf("audio not working: %s\n", Mix_GetError());
    exit(-1);
  }

  if ((music = Mix_LoadMUS("assets/nightmare.wav")) == NULL) {
    printf("Could not load music file: %s\n", Mix_GetError());
    exit(-1);
  }

  // SET MONITOR
  int useDisplay = 0; // which monitor to use
  int displays = SDL_GetNumVideoDisplays();
  printf("displays: %d\n", displays);

  SDL_Rect *displayBounds = malloc(sizeof(SDL_Rect) * displays);
  for (int i = 0; i < displays; i++) {
    SDL_GetDisplayBounds(i, &displayBounds[i]);
    printf("display bounds: %d -> x: %d, y: %d\n", i, displayBounds[i].x, displayBounds[i].y);
  }

  SDL_Window *window = NULL;
  Uint32 flags = SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  window = SDL_CreateWindow("D2_Engine", displayBounds[useDisplay].x, displayBounds[useDisplay].y,
      width, height, flags);

  if (!window) {
    printf("ERROR: Window init failed\n");
    exit(0);
  }

  // SDL_GLContext glcontext = SDL_GL_CreateContext(window);
  SDL_GL_CreateContext(window);

  if (!gladLoadGLLoader((void *)SDL_GL_GetProcAddress)) {
    printf("ERROR: Could not initialize glad.\n");
    exit(0);
  }

  printf("Vendor: %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version: %s\n", glGetString(GL_VERSION));
  printf("Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  free(displayBounds);

  engine->window = window;
}

void Audio_PlayMusic() {
  Mix_PlayMusic(music, -1);
}

void _Backend_HandleEvents() {

  SDL_Event event;

  while (SDL_PollEvent(&event)) {

    switch (event.type) {

      // case SDL_WINDOWEVENT: {
      //   if (engine->event.window.event == SDL_WINDOWEVENT_RESIZED) {
      //     // printf("w: %d, h: %d\n", window_width, window_height);
      //     // Mat4_print("proj", m.projection);
      //     window_width = engine->event.window.data1;
      //     window_height = engine->event.window.data2;
      //     glViewport(0, 0, window_width, window_height);
      //     _Engine_UpdateProjectionMatrix();
      //     globalScale = window_width / 800.0;
      //     // Mat4_print("proj", m.projection);
      //     // printf("w: %d, h: %d\n", window_width, window_height);
      //     // Mat4_print("VP", m.vp);
      //   }
      // } break;

    case SDL_QUIT: {
      engine->isRunning = 0;
    } break;

    case SDL_MOUSEMOTION: {
      mouse.x = event.button.x;
      mouse.y = Engine_GetWindowHeight() - event.button.y;
    } break;

    case SDL_MOUSEBUTTONDOWN: {
      mouse.button[event.button.button] = true;
      mouse.x = event.button.x;
      mouse.y = Engine_GetWindowHeight() - event.button.y;
      printf("mouse down: %d -> %f,%f\n", event.button.button, mouse.x, mouse.y);
    } break;

    case SDL_MOUSEBUTTONUP: {
      mouse.button[event.button.button] = false;
      mouse.x = event.button.x;
      mouse.y = Engine_GetWindowHeight() - event.button.y;
      printf("mouse up: %f,%f\n", mouse.x, mouse.y);

    } break;

    case SDL_KEYDOWN: {
      SDL_Scancode code = event.key.keysym.scancode;
      printf("keycode: %d\n", code);
      keys[code] = true;
    } break;

    case SDL_KEYUP: {
      SDL_Scancode code = event.key.keysym.scancode;
      keys[code] = false;
    } break;
    }
  }

  if (Engine_GetKeyPressedOnce(D2_KEY_F5)) {
    engine->isRunning = 0;
  }
}

void _Backend_SwapBuffers() { SDL_GL_SwapWindow(engine->window); }

u16 Engine_GetDelta() { return deltaInMillis; }

bool Engine_GetIsRunning() { return engine->isRunning; }

void _Backend_InitTime() {
  startTime = SDL_GetTicks() / 1000.0f;
  endTime = SDL_GetTicks() / 1000.0f;
}

void _Backend_CalculateDelta() {
  endTime = SDL_GetTicks() / 1000.0f;
  float deltaInSeconds = endTime - startTime;
  deltaInMillis = deltaInSeconds * 1000;
  startTime = endTime;

  frames++;
  timeSinceLastFpsPrint += deltaInSeconds;
  if (timeSinceLastFpsPrint >= 1) {
    // printf("FPS: %f\n", frames / timeSinceLastFpsPrint);
    timeSinceLastFpsPrint = 0;
    frames = 0;
  }
}

// void Engine_ResizeWindow(int w, int h) {
//   SDL_Engine_SetWindowFullscreen(engine->window, 0);
//   SDL_SetWindowSize(engine->window, w, h);
//   window_width = w;
//   window_height = h;
//   glViewport(0, 0, window_width, window_height);
//   _Engine_UpdateProjectionMatrix();
//   globalScale = window_width / 1920.0;
// }

// void Engine_SetWindowFullscreen() {
//   SDL_Engine_SetWindowFullscreen(engine->window,
//   SDL_WINDOW_FULLSCREEN_DESKTOP);
// }

#endif
