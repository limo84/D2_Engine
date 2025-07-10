#include "d2.h"
#include "d2_structs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

Sprite *Sprite_New(Texture *texture, u32 x, u32 y) {
  Sprite *self = (Sprite *)calloc(1, sizeof(Sprite));
  self->texture = texture;

  self->animationFrame = 0;
  //   self->animationFrameMax = 1;
  self->flipTextureX = false;
  self->flipTextureY = false;

  self->position.x = x;
  self->position.y = y;
  self->rotation = 0;
  self->alpha = 1;

  self->width = texture->width;
  self->height = texture->height;
  self->animationTimer = Timer_New(0);
  self->currentAnimation = NULL;
  self->nextAnimation = NULL;

  self->animationsAmount = 0;
  self->animations = NULL;
//   self->animation[0] = (Animation){{0, 0}, {texture->width, texture->height}, 1};
  return self;
}

Sprite *Sprite_Free(Sprite *self) {
  free(self);
  return NULL;
}

void Sprite_AnimationAlloc(Sprite *self, u8 amount) {
  self->animationsAmount = amount;
  self->animations = calloc(amount, sizeof(Animation));
}

void Sprite_AddAnimation(Sprite *self, char *name, Vec2 start, Vec2 dimensions, u8 frames, u16 duration) {
  for (int i = 0; i < self->animationsAmount; i++) {
    if (self->animations[i].name == NULL) {
      self->animations[i].name = malloc(strlen(name) + 1);
      memcpy(self->animations[i].name, name, strlen(name) + 1);
      self->animations[i].start = start;
      self->animations[i].dimensions = dimensions;
      self->animations[i].frames = frames;
      self->animations[i].duration = duration;
      break;
    }
  }
}

Animation* Sprite_GetAnimation(Sprite *self, char *name) {
  for (int i = 0; i < self->animationsAmount; i++) {
    assert(self->animations[i].name);
    if (strcmp(self->animations[i].name, name) == 0) {
      return &self->animations[i];
    }
  }
  printf("ERROR: No Animation with Name: %s\n", name);
  return NULL;
}

void Sprite_FadeOut(Sprite *self, int millis) {
  // if (self->texture->alpha <= 0) {
  //   // remove
  //   return;
  // }
  self->alpha = 0;
}

void Sprite_SetAnimation(Sprite *self, char name[ANIMATION_NAME_LENGTH], bool loop) {
  self->loopAnimation = loop;
  if (self->currentAnimation != NULL && strcmp(self->currentAnimation->name, name) == 0) {
    return;
  }
  printf("asd\n");
  self->animationFrame = 0;
  self->currentAnimation = Sprite_GetAnimation(self, name);
  self->nextAnimation = NULL;
  Timer_Reset(self->animationTimer, self->currentAnimation->duration);
}

void Sprite_PlayAnimationOnce(Sprite *self, char name[ANIMATION_NAME_LENGTH], char nextAnimName[ANIMATION_NAME_LENGTH]) {
  if (self->currentAnimation != NULL && strcmp(self->currentAnimation->name, name) == 0) {
    return;
  }
  self->currentAnimation = Sprite_GetAnimation(self, name);
  self->nextAnimation = Sprite_GetAnimation(self, nextAnimName);
  Timer_Reset(self->animationTimer, self->currentAnimation->duration);
}

void Sprite_Update(Sprite *self) {


  if (self->currentAnimation == NULL) {
    return;
  }

  Timer_Update(self->animationTimer);

  // printf("huhuu %lu\n", self->animationTimer->currentTime);

  // printf("%lu\n", Timer_getTime(self->animationTimer));
  if (self->animationTimer->elapsed) {

    bool animationHasEnded = self->currentAnimation->frames == self->animationFrame + 1;

    if (animationHasEnded && !self->loopAnimation) {
      return;
    }

    self->animationFrame++;

    if (animationHasEnded && self->nextAnimation != NULL) {
      self->currentAnimation = self->nextAnimation;
      self->animationFrame = 0;
      self->nextAnimation = NULL;
    }

    self->animationFrame %= self->currentAnimation->frames;
    // printf("anim: %d\n", self->animationFrame);

    Timer_Reset(self->animationTimer, self->currentAnimation->duration);
  }
}

void Sprite_SetSize(Sprite *self, int width, int height) {
  self->width = width;
  self->height = height;
}

void Sprite_SetRotation(Sprite *self, float rotation) { self->rotation = rotation; }

void Sprite_ChangeScalation(Sprite *self, float scale) {
  self->width = scale * self->texture->width;
  self->height = scale * self->texture->height;
}

void sprite_set_texture(Sprite *self, char *fileName) {
  // self->texture = Texture_LoadFromFile(fileName);
}

void sprite_set_animation_frame(Sprite *self, u8 animationFrame) { self->animationFrame = animationFrame; }
