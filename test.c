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
}

int main() {
  Engine_Init();
  Engine_OpenWindow(640, 480, true);
  tex = Texture_LoadFromFile("assets/tex_GOL_StartButton.png");
  sprite = Sprite_New(tex, 400, 400);
  Engine_RunMainloop(mainloop);
  return 0;
}
