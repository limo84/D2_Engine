// TODO
// [?] allow texture loading before window is open
// create pixelfont by name
// [X] hex colorstrings
// [X] DrawLine, DrawPolygon does not draw...
// DrawCircle, DrawRectangle
// if a line is drawn after a sprite, the sprite isnt textured...
// get monitor dimensions before window is opened

#include "src/d2.h"

Sprite *sprite = NULL;
Sprite *sprite2 = NULL;
Rect rect = {400, 100, 400, 400};

void mainloop() {
  Engine_DrawRectangle("00ff00ff", rect);
  Sprite_Update(sprite);
  Engine_DrawLine("ffffffff", (Vec2) {100, 100}, (Vec2) {1000, 1000});
  Engine_DrawPolygon("ab231fff", 3, (Vec2){100, 100}, (Vec2){500, 100}, (Vec2){500, 300});
  Sprite_Draw(sprite2);
  Sprite_Draw(sprite);
}

int main() {
  Engine_Init();
  Engine_OpenWindow(800, 600, false);

  // LOAD A TEXTURE
  Texture *texture = NULL;
  char fileName[256];
  sprintf(fileName, "assets/tex_player.png");
  texture = Texture_LoadFromFile(fileName);
  if (texture == NULL) {
    logger_log("huhu");
    exit(0);
  }
  // CREATE A SPRITE
  sprite = Sprite_New(texture, 200, 200);

  // LOAD A TEXTURE
  sprintf(fileName, "assets/tex_GOL_StartButton.png");
  texture = Texture_LoadFromFile(fileName);
  if (texture == NULL) {
    logger_log("huhu");
    exit(0);
  }
  // CREATE A SPRITE
  sprite2 = Sprite_New(texture, 200, 150);

  // MAIN LOOP
  Engine_RunMainloop(mainloop);
  return 0;
}
