#define LSH
#include "lsh_logger.h"
#include "d2.h"
#include "d2_keys.h"
#include "d2_priv.h"
#include "d2_structs.h"
#include "glad.h"
#include "shaders.txt"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265

#define LOG(fmt, ...) logger_log(fmt, ...)

int window_width;
int window_height;

typedef struct {
  unsigned int model;
  unsigned int vp;
  unsigned int animationFrame;
  unsigned int animationFrameMax;
  unsigned int scale; // unused ??
  unsigned int recOnly;
  unsigned int color;
  unsigned int texture;
  unsigned int texOffset;
} Locations;

typedef struct {
  Locations locations;
  unsigned int VBO; // ...
  unsigned int VAO; // ...
  unsigned int EBO; // ...
  unsigned int shader;
} OpenGl;

OpenGl ogl;

unsigned int polyVAO, polyVBO;

Matrices m;

bool keys[512];
bool oldKeys[512];
Mouse mouse;
Mouse oldMouse;

float cameraPosX = 0;
char *path;

bool e_released = true;

float globalScale = 1.0;

unsigned int _defaultShader();

void Engine_Init() {}

void Engine_OpenWindow(u32 width, u32 height, bool fullscreen) {
  _Backend_GetPath(path);
  window_width = width;   // TODO remove
  window_height = height; // TODO remove
  logger_set_filename("d2log.log");
  logger_log("logging works in d2...\n");
  logger_log("PATH1: %s\n", path);
  _Backend_CreateWindow(width, height, fullscreen);
  _Engine_CreateQuadVAO();
  _Engine_CreatePolyVAO();
  ogl.shader = _defaultShader();
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(ogl.shader);
  srand(1);
  _Engine_InitMatrices();
  _Engine_InitUniformLocs();
}

void Engine_RunMainloop(void (*mainloopFunction)(void)) {
  _Backend_InitTime();
  while (Engine_GetIsRunning()) {
    _Backend_HandleEvents();
    _Backend_CalculateDelta();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    mainloopFunction();
    _Backend_SwapBuffers();
    Engine_UpdateOldKeys();
    oldMouse = mouse;
  }
}

void Sprite_Draw(Sprite *self) { Sprite_DrawAt(self, self->position, self->rotation); }

void Sprite_DrawAt(Sprite *self, Vec2 position, float rotation) {
  float sizeX = (self->flipTextureX ? -1 : 1) * self->width / 100.0f * globalScale;
  float sizeY = (self->flipTextureY ? 1 : -1) * self->height / 100.0f * globalScale;
  Mat4_set_rotation(m.rotate, Engine_DegreeToRadians(rotation));
  Mat4_set_translation(m.translate, position.x + self->width * globalScale / 2.0f,
      position.y + self->height * globalScale / 2.0f, 0);
  Mat4_set_scalation(m.scale, sizeX, -sizeY, 1); // TODO
  Mat4_multiply(m.rotate, m.scale, m.model);
  Mat4_multiply(m.translate, m.model, m.model);
  glUniform1i(ogl.locations.recOnly, 0);
  float offsetX = 0;
  float offsetY = 0;
  float scaleX = 1;
  float scaleY = 1;
  if (self->currentAnimation != NULL) {
    offsetX = self->animationFrame * (float)self->currentAnimation->dimensions.x / 
        self->texture->width;
    offsetY = self->currentAnimation->start.y / self->texture->height;
    scaleX = self->currentAnimation->dimensions.x / self->texture->width;
    scaleY = self->currentAnimation->dimensions.y / self->texture->height;
  }
  float alpha = self->alpha;
  glUniform4f(ogl.locations.texOffset, offsetX, offsetY, scaleX, scaleY);
  glUniform4f(ogl.locations.color, alpha, alpha, alpha, alpha);
  glBindTexture(GL_TEXTURE_2D, self->texture->id);
  glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, m.model);
  glBindVertexArray(ogl.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Texture_DrawAt(Texture *self, Vec2 position) {

  float sizeX = self->width / 100.0f * globalScale;
  float sizeY = self->height / 100.0f * globalScale;

  Mat4_set_rotation(m.rotate, 0);
  // +50 hier ist eher sprite.width * sprite.scale / 2
  Mat4_set_translation(m.translate, position.x + self->width * globalScale / 2.0f,
      position.y + self->height * globalScale / 2.0f, 0);
  Mat4_set_scalation(m.scale, sizeX, -sizeY, 1); // TODO

  Mat4_multiply(m.rotate, m.scale, m.model);
  Mat4_multiply(m.translate, m.model, m.model);

  glUniform1i(ogl.locations.recOnly, 0);
  glUniform1i(ogl.locations.animationFrameMax, 1);
  glUniform1i(ogl.locations.animationFrame, 1);
  glUniform4f(ogl.locations.color, 1, 1, 1, 1);

  glBindTexture(GL_TEXTURE_2D, self->id);
  glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, m.model);
  glBindVertexArray(ogl.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Texture_Free(Texture *self) { free(self); }

PixelFont *PixelFont_New(Texture *texture, u8 frameWidth, u8 frameHeight, u8 spacing) {
  PixelFont *self = malloc(sizeof(PixelFont));
  self->texture = texture;
  self->frameWidth = frameWidth;
  self->frameHeight = frameHeight;
  self->spacing = spacing;
  return self;
}

void _DrawCharacter(PixelFont *font, char c, Color color, int x, int y, float scale) {
  float sizeX = font->frameWidth / 100.0f * scale * globalScale; // WARUM DURCH 100 ??? WEIL VERTICES WIDTH = 100
  float sizeY = font->frameHeight / 100.0f * scale * globalScale;
  int index = c - 32;
  Mat4_set_rotation(m.rotate, 0);
  Mat4_set_translation(m.translate, x, y, 0);
  Mat4_set_scalation(m.scale, sizeX, sizeY, 1);
  Mat4_multiply(m.rotate, m.scale, m.model);
  Mat4_multiply(m.translate, m.model, m.model);
  glUniform1i(ogl.locations.recOnly, 0);
  // glUniform1i(ogl.locations.animationFrameMax, font->size);
  // glUniform1i(ogl.locations.animationFrame, index);
  glUniform4f(ogl.locations.color, color.r, color.g, color.b, color.a);
  float offsetX = index * (float)font->frameWidth / font->texture->width;
  float offsetY = 0;
  float scaleX = (float)font->frameWidth / font->texture->width;
  float scaleY = (float)font->frameHeight / font->texture->height;
  glUniform4f(ogl.locations.texOffset, offsetX, offsetY, scaleX, scaleY);
  glBindTexture(GL_TEXTURE_2D, font->texture->id);
  glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, m.model);
  glBindVertexArray(ogl.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Engine_DrawText(PixelFont *font, Color color, char *Text, int x, int y, float scale) {
  for (int i = 0; Text[i]; i++) {
    _DrawCharacter(font, Text[i], color, x + globalScale * scale * i * (font->frameWidth + font->spacing), y, scale);
  }
}

float Engine_DegreeToRadians(float degree) { return degree / 180 * PI; }

void _Engine_CreatePolyVAO() {
  float vertices[] = {
      10, 10, 0,  //
      0, 100, 0,  //
      100, 50, 0, //
      100, 0, 0,  //
      200, 200, 0 //
  };
  unsigned int indices[] = {
      0, 1, 2,
      0, 2, 3,
      0, 3, 4,
      0, 4, 5
  };
  GLuint ebo;
  glGenVertexArrays(1, &polyVAO);
  glGenBuffers(1, &polyVBO); // Generate 1 buffer
  glGenBuffers(1, &ebo);
  glBindVertexArray(polyVAO);
  glBindBuffer(GL_ARRAY_BUFFER, polyVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbinds the VBO, not required ?
  glBindVertexArray(0);             // unbinds the VAO
}

Color _ColorHexToRGBA(const char *colorHex) {
  u8 len = strlen(colorHex);
  if (len != 6 && len != 8) {
    logger_log("wrong color string\n");
    return (Color){0, 0, 0, 255};
  }
  u8 values[4] = {0};
  char buf[3];
  for (int i = 0; i < len / 2; i++) {
    strncpy(buf, colorHex + 2 * i, 2);
    values[i] = ((u8) strtoul(buf, NULL, 16));
  }
  return (Color) {values[0], values[1],values[2], values[3]};
}

void Engine_DrawLine(const char *colorHex, Vec2 a, Vec2 b) {
  float vertices[] = {a.x, a.y, 0, b.x, b.y, 0};

  glUniform1i(ogl.locations.recOnly, 1);
  // glUniform1i(ogl.locations.animationFrameMax, 1);
  // glUniform1i(ogl.locations.animationFrame, 1);

  float *unit = Mat4_CreateIdentity();
  glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, unit);

  glBindVertexArray(polyVAO);

  glBindBuffer(GL_ARRAY_BUFFER, polyVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
  
  Color color = _ColorHexToRGBA(colorHex);
  glUniform4f(ogl.locations.color, color.r, color.g, color.b, color.a);
  glDrawElements(GL_LINES, 3, GL_UNSIGNED_INT, 0);
}

void Engine_DrawRectangle(const char *colorHex, Rect rect) {
  float sizeX = rect.w / 100.0f * globalScale;
  float sizeY = rect.h / 100.0f * globalScale;
  Mat4_set_rotation(m.rotate, 0);
  Mat4_set_translation(m.translate, rect.x + rect.w * globalScale / 2.0f,
      rect.y + rect.h * globalScale / 2.0f, 0);
  Mat4_set_scalation(m.scale, sizeX, -sizeY, 1);
  Mat4_multiply(m.rotate, m.scale, m.model);
  Mat4_multiply(m.translate, m.model, m.model);
  glUniform1i(ogl.locations.recOnly, 1);
  Color color = _ColorHexToRGBA(colorHex);
  glUniform4f(ogl.locations.color, color.r / 255.0, color.g / 255.0,
    color.b / 255.0, color.a / 255.0);
  glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, m.model);
  glBindVertexArray(ogl.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Engine_DrawPolygon(const char *colorHex, int amount, ...) {

  float vertices[] = {
      80, 100, 0,  //
      100, 200, 0, //
      200, 150, 0, //
      200, 100, 0, //
      0, 0, 0,     //
      0, 0, 0      //
  };

  va_list valist;
  va_start(valist, amount);
  for (int i = 0; i < amount; i++) {
    Vec2 vec = va_arg(valist, Vec2);
    vertices[i * 3] = vec.x;
    vertices[i * 3 + 1] = vec.y;
  }
  va_end(valist);

  Color color = _ColorHexToRGBA(colorHex);
  glUniform4f(ogl.locations.color, color.r / 255.0, color.g / 255.0,
    color.b / 255.0, color.a / 255.0);
  glUniform1i(ogl.locations.recOnly, 1);
  float *unit = Mat4_CreateIdentity();
  glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, unit);
  glBindVertexArray(polyVAO);
  glBindBuffer(GL_ARRAY_BUFFER, polyVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  /// use GL_TRIANGLE_FAN ?
  glDrawElements(GL_TRIANGLES, (amount - 2) * 3, GL_UNSIGNED_INT, 0);
  free(unit);
}

Color Engine_GetRandomColor() {
  float r = (rand() % 256) / 256.0f;
  float g = (rand() % 256) / 256.0f;
  float b = (rand() % 256) / 256.0f;

  return (Color){r, g, b, 1.0f};
}

bool Engine_IsPointInsidePolygon(Vec2 *pt, Vec2 *pt_list, unsigned short n_pt) {
  float x = pt->x, y = pt->y;

  bool inside = false;
  for (int i = 0, j = n_pt - 1; i < n_pt; j = i++) {
    float xi = pt_list[i].x, yi = pt_list[i].y;
    float xj = pt_list[j].x, yj = pt_list[j].y;

    bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
    if (intersect) {
      inside = !inside;
    }
  }

  return inside;
}

void Engine_UpdateCamera(float x, float y) {
  cameraPosX = x - (float)window_width / 2;
  if (cameraPosX < 0) {
    cameraPosX = 0;
  }
  // printf("cam x: %f\n", cameraPosX);
  Mat4_set_translation(m.view, -cameraPosX, 0.0, 0);
  Mat4_multiply(m.projection, m.view, m.vp);
  // Mat4_print("m.projection", m.projection);
  glUniformMatrix4fv(ogl.locations.vp, 1, GL_TRUE, m.vp);
}

u32 Engine_GetWindowWidth() { return window_width; }

u32 Engine_GetWindowHeight() { return window_height; }

float Engine_GetGlobalScale() { return globalScale; }

void Engine_UpdateOldKeys() {
  for (int i = 0; i < 512; i++) {
    oldKeys[i] = keys[i];
  }
}

Timer *Timer_New(u64 millis) {
  Timer *self = malloc(sizeof(Timer));
  self->active = true;
  self->elapsed = false;
  self->startTime = millis;
  self->currentTime = millis;
  return self;
}

void Timer_Update(Timer *self) {
  if (!self->active) {
    return;
  }
  // printf("delta %d\n", Engine_GetDelta());
  self->currentTime -= Engine_GetDelta();
  if (self->currentTime <= 0) {
    self->elapsed = true;
  }
}

void Timer_Reset(Timer *self, u64 millis) {
  self->elapsed = false;
  self->startTime = millis;
  self->currentTime = millis;
}


bool Engine_GetKeyPressedOnce(int key) { return !oldKeys[key] && keys[key]; }

bool Mouse_GetPressed(int button) { return mouse.button[button]; }

bool Mouse_GetPressedOnce(int button) { return mouse.button[button] && !oldMouse.button[button]; }

float Mouse_GetPositionX() { return mouse.x; }
float Mouse_GetPositionY() { return mouse.y; }

bool Mouse_IsInsideRect(Rect rect) {
  return mouse.x >= rect.x && mouse.x <= rect.x + rect.w && 
    mouse.y >= rect.y && mouse.y <= rect.y + rect.h;
}

void _Engine_InitMatrices() {

  m.projection = Mat4_CreateIdentity(); // converts clipping space to screen coords
  m.view = Mat4_CreateIdentity(); // "moves camera" or rather all objects (to l if cam moves r)
  m.vp = Mat4_CreateIdentity();
  m.scale = Mat4_CreateIdentity();
  m.translate = Mat4_CreateIdentity();
  m.rotate = Mat4_CreateIdentity();
  m.model = Mat4_CreateIdentity();

  Mat4_set_translation(m.view, 0, 0, 0);
  Mat4_multiply(m.projection, m.view, m.vp);
  // Mat4_print("m.projection", m.projection);
  glUniformMatrix4fv(ogl.locations.vp, 1, GL_TRUE, m.vp);

  // TODO also when window gets resized
  m.projection[0] = 2.0f / window_width;
  m.projection[3] = -1.0f;
  m.projection[5] = 2.0f / window_height;
  m.projection[7] = -1.0f;

  // printf("projection:\n");

  Mat4_set_translation(m.translate, 0.0, 0.0, 0);
  Mat4_multiply(m.translate, m.scale, m.model);

  Mat4_print("model", m.model);
  Mat4_print("scale", m.scale);
  Mat4_print("translate", m.translate);
  Mat4_print("projection", m.projection);
}

void _Engine_UpdateProjectionMatrix() {
  m.projection[0] = 2.0f / window_width;
  m.projection[3] = -1.0f;
  m.projection[5] = 2.0f / window_height;
  m.projection[7] = -1.0f;
  Mat4_print("VP", m.vp);
  Mat4_multiply(m.projection, m.view, m.vp);
  Mat4_print("VP", m.vp);
  glUniformMatrix4fv(ogl.locations.vp, 1, GL_TRUE, m.vp);
}

void _Engine_InitUniformLocs() {
  ogl.locations.model = glGetUniformLocation(ogl.shader, "model");
  ogl.locations.vp = glGetUniformLocation(ogl.shader, "vp");
  ogl.locations.color = glGetUniformLocation(ogl.shader, "shaderColor");
  ogl.locations.texture = glGetUniformLocation(ogl.shader, "ourTexture");
  ogl.locations.animationFrame = glGetUniformLocation(ogl.shader, "animationFrame");
  ogl.locations.animationFrameMax = glGetUniformLocation(ogl.shader, "animationFrameMax");
  // flipTextureLoc = glGetUniformLocation(ogl.shader, "flipTexture");
  ogl.locations.recOnly = glGetUniformLocation(ogl.shader, "recOnly");
  ogl.locations.texOffset = glGetUniformLocation(ogl.shader, "textureOffset");

  // glUniform4f(ogl.locations.color, 1.0f, 0.0f, 0.0f, 1.0f);
  glUniformMatrix4fv(ogl.locations.vp, 1, GL_TRUE, m.projection);
  glUniform1i(ogl.locations.texture, 0);
  glUniform1i(ogl.locations.animationFrame, 0);
  // glUniform1i(flipTextureLoc, 0);
  glUniform1i(ogl.locations.recOnly, 0);
}

void print_transform_locs() {
  printf("shader id: %d\n", ogl.shader);
  printf("transform loc: %d\n", ogl.locations.model);
  printf("vp loc: %d\n", ogl.locations.vp);
  printf("color loc: %d\n", ogl.locations.color);
  // printf("texture loc: %d\n", ogl.locations.texture);
  printf("animationFrame loc: %d\n", ogl.locations.animationFrame);
  printf("animationFrameMax loc: %d\n", ogl.locations.animationFrameMax);
  // printf("flipTexture loc: %d\n", flipTextureLoc);
}

Texture *Texture_LoadFromFile(char const *filename) {
  logger_log("PATH1: %s\n", path);
  logger_log("PATH1: %s\n", filename);
  Texture *self = (Texture *)malloc(sizeof(Texture));
  // unsigned int texture;
  glGenTextures(1, &self->id);
  glBindTexture(GL_TEXTURE_2D, self->id);

  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // load and generate the texture
  // int width, height, nrChannels;
  int nrChannels;

  stbi_set_flip_vertically_on_load(1); // TODO call only once?
  unsigned char *data = stbi_load(filename, &self->width, &self->height, &nrChannels, 0);
  if (data) {
    logger_log("Loaded TEXTURE: %s. Width: %d, Height: %d\n", filename, self->width, self->height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, self->width, self->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    logger_log("TEXTURE failed: %s\n", filename);
    return NULL;
  }
  stbi_image_free(data);

  // self->alpha = 1.0f;

  return self;
}

Button *Button_New(Texture *texture, Rect src, Rect dest, void (*OnMouseEnter)(Button *self),
    void (*OnMouseLeave)(Button *self), void (*OnClick)(Button *self, void *data)) {
  Button *self = malloc(sizeof(Button));
  self->texture = texture;
  self->src = src;
  self->dest = dest;
  self->hovered = false;
  self->visible = true;
  self->OnMouseEnter = OnMouseEnter;
  self->OnMouseLeave = OnMouseLeave;
  self->OnClick = OnClick;
  return self;
}

void Button_Update(Button *self, void *data) {
  if (!self->visible) {
    return;
  }

  if (!self->hovered && Mouse_IsInsideRect(self->dest)) {
    self->hovered = true;
    if (self->OnMouseEnter) {
      self->OnMouseEnter(self);
    }
  }

  else if (self->hovered && !Mouse_IsInsideRect(self->dest)) {
    self->hovered = false;
    if (self->OnMouseLeave) {
      self->OnMouseLeave(self);
    }
    printf("mouse left\n");
  }

  if (Mouse_GetPressedOnce(D2_MOUSE_BUTTON_1) && Mouse_IsInsideRect(self->dest)) {
    printf("click\n");
    if (self->OnClick && data) {
      self->OnClick(self, data);
    }
  }
}

void Button_Draw(Button *self) {
  if (!self->visible) {
    return;
  }

  float sizeX = 2 * self->dest.w / 100.0f * globalScale;
  float sizeY = 2 * self->dest.h / 100.0f * globalScale;

  Mat4_set_rotation(m.rotate, 0);
  // +50 hier ist eher sprite.width * sprite.scale / 2
  Mat4_set_translation(m.translate, self->dest.x + self->dest.w * globalScale / 2.0f,
      self->dest.y + self->dest.h * globalScale / 2.0f, 0);
  Mat4_set_scalation(m.scale, sizeX, sizeY, 1); // TODO

  Mat4_multiply(m.rotate, m.scale, m.model);
  Mat4_multiply(m.translate, m.model, m.model);

  glUniform1i(ogl.locations.recOnly, 0);
  // glUniform1i(ogl.locations.animationFrameMax, 1);
  // glUniform1i(ogl.locations.animationFrame, 1);
  glUniform4f(ogl.locations.color, 1, 1, 1, 1);
  // glUniform1i(flipTextureLoc, self->flipTextureX);

  // float offsetX = (float)self->src.w / self->texture->width;
  float offsetX = self->src.x / self->texture->width;
  float offsetY = 0;
  float scaleX = (float)self->src.w / self->texture->width;
  float scaleY = (float)self->src.h / self->texture->height;

  glUniform4f(ogl.locations.texOffset, offsetX, offsetY, scaleX, scaleY);
  glBindTexture(GL_TEXTURE_2D, self->texture->id);
  glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, m.model);
  glBindVertexArray(ogl.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Sprite_DrawHitbox(Sprite *self) {
  // Mat4_set_scalation(m.scale, self->hitbox.w / 100, self->hitbox.h / 100, 1);
  // Mat4_set_translation(m.translate,
  //                      self->position.x + self->hitbox.x + self->hitbox.w /
  //                      2, self->position.y + 50, 0);

  Mat4_multiply(m.translate, m.scale, m.model);

  glUniform1i(ogl.locations.recOnly, 1);
  // glUniform1i(ogl.locations.animationFrameMax, self->animationFrameMax);
  // glUniform1i(ogl.locations.animationFrame, self->animationFrame);
  // glUniform1i(flipTextureLoc, self->flipTexture);

  // glBindTexture(GL_TEXTURE_2D, self->texture);
  glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, m.model);
  glBindVertexArray(ogl.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// float sprite_get_hitbox_bottom(Sprite *self) {
//   return self->position.y + self->hitbox.y;
// }

// float sprite_get_hitbox_top(Sprite *self) {
//   return self->position.y + self->hitbox.y + self->hitbox.h;
// }

// float sprite_get_hitbox_left(Sprite *self) {
//   return self->position.x + self->hitbox.x;
// }

// float sprite_get_hitbox_right(Sprite *self) {
//   return self->position.x + self->hitbox.x + self->hitbox.w;
// }

// void texture_render(u8 texture, float pos_x, float pos_y, float scale_x,
//                     float scale_y) {

//   glUniform1i(ogl.locations.recOnly, 0);
//   glUniform1i(ogl.locations.animationFrameMax, 1);
//   glUniform1i(flipTextureLoc, 0);
//   glBindTexture(GL_TEXTURE_2D, texture);

//   Mat4_set_rotation(m.rotate, 0.0);
//   Mat4_set_translation(m.translate, pos_x + 50, pos_y + 50, 0);
//   m.model = Mat4_multiply(m.translate, m.rotate);
//   Mat4_set_scalation(m.scale, scale_x, scale_y, 1);
//   m.model = Mat4_multiply(m.scale, m.model);

//   glUniformMatrix4fv(ogl.locations.model, 1, GL_TRUE, m.model);
//   glBindVertexArray(ogl.VAO);
//   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
// }

char *readShaderFileToString(const char *fileName) {
  FILE *file = NULL;
  char *buffer = 0;
  int length;

  file = fopen(fileName, "rb");

  printf("Read shader: %s\n", fileName);

  if (file == NULL) {
    printf("[ERROR] File not found: %s\n", fileName);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  length = ftell(file);
  fseek(file, 0, SEEK_SET);
  buffer = malloc(length + 1);

  if (buffer == NULL) {
    printf("[ERROR] No buffer!\n");
    exit(1);
  }

  fread(buffer, 1, length, file);
  buffer[length] = '\0';
  fclose(file);
  return buffer;
}

unsigned int _defaultShader() {
  unsigned int shader;
  // create vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &defaultVertex, NULL);
  glCompileShader(vertexShader);
  // error checking [needed?]
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
  }
  // create fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &defaultFragment, NULL);
  glCompileShader(fragmentShader);
  // error checking
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
  }
  // create shader program and link shaders
  shader = glCreateProgram();
  glAttachShader(shader, vertexShader);
  glAttachShader(shader, fragmentShader);
  glLinkProgram(shader);
  // check for linking errors
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader, 512, NULL, infoLog);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
  }
  // since they are in program now, we no longer need the shaders..
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return shader;
}


unsigned int Shader_New(const char *vertexPath, const char *fragmentPath) {
  unsigned int shader;
  const char *vertexShaderSource = readShaderFileToString(vertexPath);
  const char *fragmentShaderSource = readShaderFileToString(fragmentPath);
  // create vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &defaultVertex, NULL);
  glCompileShader(vertexShader);
  // error checking
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
  }
  // create fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &defaultFragment, NULL);
  glCompileShader(fragmentShader);
  // error checking
  // memset(infoLog, 0, 512);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
  }
  // create shader program and link shaders
  shader = glCreateProgram();
  glAttachShader(shader, vertexShader);
  glAttachShader(shader, fragmentShader);
  glLinkProgram(shader);
  // check for linking errors
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader, 512, NULL, infoLog);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
  }
  // since they are in program now, we no longer need the shaders..
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  free((void *)vertexShaderSource);
  free((void *)fragmentShaderSource);
  return shader;
}

void _Engine_CreateQuadVAO() {
  float vertices[] = {
      // pos                            // texture
      -50.0f, -50.0f, 0.0f, 0.0f, 0.0f, // bottom left
      50.0f, -50.0f, 0.0f, 1.0f, 0.0f,  // bottom right
      50.0f, 50.0f, 0.0f, 1.0f, 1.0f,   // top right
      -50.0f, 50.0f, 0.0f, 0.0f, 1.0f   // top left
  };
  unsigned int indices[] = {
      0, 1, 2, // first triangle
      0, 2, 3  // second triangle
  };
  glGenVertexArrays(1, &ogl.VAO); // generates 1 buffer for the VAO
  glGenBuffers(1, &ogl.VBO);      // generates 1 buffer for the VBO
  glGenBuffers(1, &ogl.EBO);      // generates 1 buffer for the EBO

  glBindVertexArray(ogl.VAO); // Binds the VAO to save all operation that are made
                          // on the VBO (and EBO ?)

  glBindBuffer(GL_ARRAY_BUFFER, ogl.VBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ogl.EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  // GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

  // Parameters of glVertexAttribPointer: (index, size, type, is_normalized,
  // stride, offset?)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0); // enables the line above ??

  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbinds the VBO, not required ??
  glBindVertexArray(0);             // unbinds the VAO
}

float *Mat4_CreateIdentity() {
  float *matrix = calloc(16, sizeof(float));
  matrix[0] = 1.0f;
  matrix[5] = 1.0f;
  matrix[10] = 1.0f;
  matrix[15] = 1.0f;
  return matrix;
}

float *Mat4_Free(float *self) {
  free(self);
  return NULL;
}

void Mat4_print(char *name, float *matrix) {
  if (name) {
    printf("%s:\n", name);
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      // printf("%.2f ", *(matrix + i * 4 + j));
      printf("%.6f ", matrix[i * 4 + j]);
    }
    printf("\n");
  }
}

void Mat4_multiply(float *mat1, float *mat2, float *result) {
  // float *temp = calloc(16, sizeof(float));
  float temp[16] = {0};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      for (int p = 0; p < 4; p++) {
        temp[i * 4 + j] += mat1[i * 4 + p] * mat2[p * 4 + j];
      }
    }
  }

  for (int i = 0; i < 16; i++) {
    result[i] = temp[i];
  }

  // free(temp);
}

void Mat4_multiply_scalar(float scalar, float *mat2, float *result) {

  for (int i = 0; i < 12; i++) {
    result[i] = scalar * mat2[i];
  }
  result[15] = 1.0f;
}

void Mat4_set_translation(float *translation_matrix, float x, float y, float z) {
  translation_matrix[3] = x;
  translation_matrix[7] = y;
  translation_matrix[11] = z;
}

void Mat4_set_scalation(float *scale_matrix, float x, float y, float z) {
  scale_matrix[0] = x;
  scale_matrix[5] = y;
  scale_matrix[10] = z;
}

void Mat4_set_rotation(float *rotation_matrix, float angle) {
  rotation_matrix[0] = cos(angle);
  rotation_matrix[1] = sin(angle);
  rotation_matrix[4] = -sin(angle);
  rotation_matrix[5] = cos(angle);
}

// bool CheckCollisionSprites(Sprite *sprite1, Sprite *sprite2) {

//   return sprite_get_hitbox_right(sprite1) >= sprite_get_hitbox_left(sprite2)
//   &&
//          sprite_get_hitbox_left(sprite1) <= sprite_get_hitbox_right(sprite2)
//          && sprite_get_hitbox_top(sprite1) >=
//          sprite_get_hitbox_bottom(sprite2) &&
//          sprite_get_hitbox_bottom(sprite1) <= sprite_get_hitbox_top(sprite2);
//   // return rec1.x + rec1.w >= rec2.x && // r1 right edge past r2 left
//   //        rec1.x <= rec2.x + rec2.w && // r1 left edge past r2 right
//   //        rec1.y + rec1.h >= rec2.y && // r1 top edge past r2 bottom
//   //        rec1.y <= rec2.y + rec2.h;
// }

void Vec2_Normalize(Vec2 *vec) {
  float length = sqrt(vec->x * vec->x + vec->y * vec->y);
  vec->x /= length;
  vec->y /= length;
}

void Engine_Cleanup() {

  m.projection = Mat4_Free(m.projection);
  m.view = Mat4_Free(m.view);
  m.model = Mat4_Free(m.model);
  m.scale = Mat4_Free(m.scale);
  m.translate = Mat4_Free(m.translate);
  m.rotate = Mat4_Free(m.rotate);

  // SDL_GL_DeleteContext((SDL_GLContext) engine->window); //
  // (http://wiki.libsdl.org/SDL_GL_DeleteContext 27)

  // SDL_DestroyWindow(
  //     engine->window);

  // free(engine);
  // SDL_Quit();
}
