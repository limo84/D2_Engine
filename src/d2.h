#pragma once

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_events.h>
// #include <SDL2/SDL_rect.h>
// #include <SDL2/SDL_scancode.h>
// #include <SDL2/SDL_stdinc.h>
// #include <SDL2/SDL_video.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "glad.h"
// #include <GLFW_3_4/glfw3.h>

#include "d2_structs.h"
#include "stb_image.h"
#include "stb_truetype.h"
#include "types.h"

#define ANIMATION_NAME_LENGTH

void Engine_Init();
void Engine_OpenWindow(u32 width, u32 height, bool fullscreen);
void Engine_Cleanup();
void Engine_RunMainloop(void (*mainloopFunction)(void));
void Engine_DrawPolygon(Color color, int amount, ...);
void Engine_DrawLine(Vec2 a, Vec2 b);
void Engine_DrawText(PixelFont *font, Color color, char *Text, int x, int y, u8 scale);
void Engine_ResizeWindow(int w, int h);
void Engine_SetWindowFullscreen();
float Engine_DegreeToRadians(float degree);
float Engine_GetGlobalScale();
bool Engine_IsPointInsidePolygon(Vec2 *pt, Vec2 *pt_list, unsigned short n_pt);
u32 Engine_GetWindowWidth();
u32 Engine_GetWindowHeight();
Color Engine_GetRandomColor();
Engine *Engine_GetEngine();
void Engine_UpdateCamera(float x, float y);
void Engine_HandleEvents();
void Engine_UpdateOldKeys();
bool Engine_GetKeyPressedOnce(int key);
u16 Engine_GetDelta();
bool Engine_GetIsRunning();
void Engine_SetIsRunning(bool setRunning);

PixelFont *PixelFont_New(Texture *texture, u8 frameWidth, u8 frameHeight);

Texture *Texture_LoadFromFile(char const *filename);
void Texture_Free(Texture *texture);
void Texture_DrawAt(Texture *texture, Vec2 position);

Button *Button_New(Texture *texture, Rect src, Rect dest, void (*OnMouseEnter)(Button *self),
    void (*OnMouseLeave)(Button *self), void (*OnClick)(Button *self, void *data));
void Button_Update(Button *self, void *data);
void Button_Draw(Button *self);

Sprite *Sprite_New(Texture *texture, u32 x, u32 y);
void Sprite_Init(Sprite *sprite, Texture *texture, u32 x, u32 y);
Sprite *Sprite_Free(Sprite *self);
void Sprite_Draw(Sprite *self);
void Sprite_DrawAt(Sprite *self, Vec2 position, float rotation);
void Sprite_Update(Sprite *self);
void Sprite_SetSize(Sprite *self, int width, int height);
void Sprite_ChangeScalation(Sprite *self, float scale);
void Sprite_SetRotation(Sprite *self, float rotation);
void sprite_set_texture(Sprite *self, char *fileName);
// void sprite_set_animation_frame(Sprite *self, u8 animationFrame);
// void sprite_set_animation_frame_max(Sprite *self, u8 animationFrameMax);
void Sprite_FadeOut(Sprite *self, int millis);
void Sprite_DrawHitbox(Sprite *self);
void Sprite_SetAnimation(Sprite *self, char *name, bool loop);
void Sprite_PlayAnimationOnce(Sprite *self, char *name, char *nextAnimationName);
// void Sprite_PlayAnimationEx(
//     Sprite *self, u8 animationType, u8 eventFrame, void (*eventFunction)(void *, void *), void *enemy, void *player);
void Sprite_AnimationAlloc(Sprite *sprite, u8 amount);
void Sprite_AddAnimation(Sprite *self, char *name, Vec2 start, Vec2 dimensions, u8 frames, u16 duration);
Animation *Sprite_GetAnimation(Sprite *self, char *name);

Timer *Timer_New(u64 millis);
// u64 Timer_getTime(Timer *self);
void Timer_Update(Timer *self);
void Timer_Reset(Timer *self, u64 millis);

bool Mouse_GetPressed(int button);
bool Mouse_GetPressedOnce(int button);
float Mouse_GetPositionX();
float Mouse_GetPositionY();
bool Mouse_IsInsideRect(Rect rect);

float *Mat4_CreateIdentity();
float *Mat4_Free(float *self);
void Mat4_print(char *, float *matrix);
void Mat4_multiply(float *mat1, float *mat2, float *result);
void Mat4_multiply_scalar(float scalar, float *mat2, float *result);
void Mat4_set_translation(float *translation_matrix, float x, float y, float z);
void Mat4_set_scalation(float *scale_matrix, float x, float y, float z);
void Mat4_set_rotation(float *rotation_matrix, float angle);

void Audio_PlayMusic();

unsigned int Shader_New(const char *vertexPath, const char *fragmentPath);

void Vec2_Normalize(Vec2 *vec);
