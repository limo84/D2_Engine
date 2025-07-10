// TODO
// allow texture loading before window is open
// create pixelfont by name
// hex colorstrings
// DrawLine, DrawPolygon does not draw...
// DrawCircle

#include "src/d2.h"

Texture *tex = NULL;
Sprite *sprite = NULL;

void mainloop() {
  Sprite_Update(sprite);
  Sprite_Draw(sprite);
  Engine_DrawLine((Vec2) {100, 100}, (Vec2) {1000, 1000});
  Engine_DrawPolygon("ab231f", 3, (Vec2){100,100}, (Vec2){500,100}, (Vec2){500,300});
}

int main() {
  Engine_Init();
  Engine_OpenWindow(1920, 1080, true);
  tex = Texture_LoadFromFile("assets/tex_GOL_StartButton.png");
  sprite = Sprite_New(tex, 400, 400);
  Engine_RunMainloop(mainloop);
  return 0;
}
