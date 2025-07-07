#pragma once

// #include <SDL2/SDL.h>
#include <stdbool.h>

#include "types.h"

typedef struct {
  unsigned int transform;
  unsigned int projection;
  unsigned int animationFrame;
  unsigned int animationFrameMax;
  unsigned int scaleLoc;
  int flipTextureLoc;
} GL_Locs;

// #ifdef SDL_WINDOW
// typedef struct {
//   SDL_Window *window;
//   SDL_Event event;
//   bool isRunning;
// } Engine;
// #endif

// typedef struct {
//   GLFWwindow *window;
//   // GLFWevent event;
//   bool isRunning;
// } Engine;
typedef struct Engine Engine;

typedef struct {
  bool active;
  bool elapsed;
  u64 startTime;
  i64 currentTime;
} Timer;

typedef struct {
  float x, y;
} Vec2;

typedef struct {
  float x, y, w, h;
} Rect;

typedef struct {
  u32 id;
  int width, height;
  // float alpha;
} Texture;

typedef struct {
  char *name;
  Vec2 start;
  Vec2 dimensions;
  u8 frames;
  u16 duration;
  // u16 *frameTimes;
} Animation;

#define MAX_ANIMS 10

typedef struct {
  Texture *texture;
  Vec2 position;
  int width;
  int height;
  float rotation;
  float alpha;
  // Vec2 velocity;
  // Rect hitbox;
  u8 animationFrame;
  // u8 animationFrameMax;
  bool flipTextureX;
  bool flipTextureY;
  // bool isAirborne;
  Timer *animationTimer;
  Animation *currentAnimation;
  Animation *nextAnimation;
  bool loopAnimation;
  Animation *animations;
  u8 animationsAmount;
} Sprite;

typedef struct _button Button;
struct _button {
  Texture *texture;
  Rect src;
  Rect dest;
  bool hovered;
  bool visible;
  void (*OnMouseEnter)(Button *self);
  void (*OnMouseLeave)(Button *self);
  void (*OnClick)(Button *self, void *data);
};

typedef struct {
  float r, g, b, a;
} Color;

typedef struct {
  bool button[8];
  double x, y;
} Mouse;

typedef struct {
  Texture *texture;
  u8 frameWidth;
  u8 frameHeight;
  // Rect src;
  // Rect dest;
  // u8 size;
} PixelFont;