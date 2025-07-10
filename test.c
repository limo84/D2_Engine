// TODO
// [?] allow texture loading before window is open
// create pixelfont by name
// [X] hex colorstrings
// [X] DrawLine, DrawPolygon does not draw...
// DrawCircle, DrawRectangle
// if a line is drawn after a sprite, the sprite isnt textured...
//

#include "src/d2.h"

void mainloop() {
  Engine_DrawLine("ffffff", (Vec2) {100, 100}, (Vec2) {1000, 1000});
  Engine_DrawPolygon("ab231f", 3, (Vec2){100, 100}, (Vec2){500, 100}, (Vec2){500, 300});
}

int main() {
  Engine_Init();
  Engine_OpenWindow(1920, 1080, true);
  Engine_RunMainloop(mainloop);
  return 0;
}
