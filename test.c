// TODO
// [?] allow texture loading before window is open
// create pixelfont by name
// [X] hex colorstrings
// [X] DrawLine, DrawPolygon does not draw...
// DrawCircle, DrawRectangle
// if a line is drawn after a sprite, the sprite isnt textured...
// get monitor dimensions before window is opened

#include "src/d2.h"
#include "src/d2_keys.h"

#define RED (Color) { 255, 0, 0, 255 }
#define BLACK (Color) { 0, 0, 0, 255 }
#define YELLOW (Color) { 255, 255, 0, 255 }
#define WHITE (Color) { 240, 240, 240, 255 }

typedef enum { MENU_RES_800x600, MENU_RES_1000x800, MENU_RES_1280x720, MENU_RES_FULLSCREEN, MENU_RES_NUM } MENU;
Sprite *sprite = NULL;
Sprite *sprite2 = NULL;
PixelFont *font = NULL;
Rect rect = {400, 100, 400, 400};
MENU menu = 0;

void drawMenu();

void mainloop() {
  Engine_DrawRectangle("00ff00ff", rect);
  Sprite_Update(sprite);
  Engine_DrawLine("ffffffff", (Vec2) {100, 100}, (Vec2) {1000, 1000});
  Engine_DrawPolygon("ab231fff", 3, (Vec2){100, 100}, (Vec2){500, 100}, (Vec2){500, 300});
  Sprite_Draw(sprite2);
  Sprite_Draw(sprite);
  drawMenu();
}

int main(int argc, char **argv) {
  Engine_Init();
  Engine_OpenWindow(800, 600, false);

  // SPRITE2__________________________________________
  Texture *texture = NULL;
  char fileName[256];
  sprintf(fileName, "assets/tex_player.png");
  texture = Texture_LoadFromFile(fileName);
  if (texture == NULL) {
    logger_log("huhu");
    exit(0);
  }
  sprite = Sprite_New(texture, 200, 200);

  // SPRITE2__________________________________________
  sprintf(fileName, "assets/tex_GOL_StartButton.png");
  texture = Texture_LoadFromFile(fileName);
  if (texture == NULL) {
    logger_log("huhu");
    exit(0);
  }
  // CREATE A SPRITE
  sprite2 = Sprite_New(texture, 200, 150);

  // FONT_____________________________________________
  sprintf(fileName, "assets/tex_font_30x60.png");
  texture = Texture_LoadFromFile(fileName);
  if (texture == NULL) {
    logger_log("huhu");
    exit(0);
  }
  font = PixelFont_New(texture, 30, 60, 2);
  // MAIN LOOP
  Engine_RunMainloop(mainloop);
  return 0;
}

void drawMenu() {
  int x = Engine_GetWindowWidth() / 2.0f - 80;
  Engine_DrawText(font, (menu == MENU_RES_800x600) ? RED : YELLOW, " 800 x 600", x, 400, 1);
  Engine_DrawText(font, (menu == MENU_RES_1000x800) ? RED : YELLOW, "1000 x 800", x, 350, 1);
  Engine_DrawText(font, (menu == MENU_RES_1280x720) ? RED : YELLOW, "1280 x 720", x, 300, 1);
  Engine_DrawText(font, (menu == MENU_RES_FULLSCREEN) ? RED : WHITE, "WWWWWwwwullscreen", x, 250, 1);

  if (Engine_GetKeyPressedOnce(D2_KEY_DOWN)) {
    menu = (menu + 1) % MENU_RES_NUM;
  }

  if (Engine_GetKeyPressedOnce(D2_KEY_UP)) {
    menu = (menu + MENU_RES_NUM - 1) % MENU_RES_NUM;
  }

  if (Engine_GetKeyPressedOnce(D2_KEY_ENTER)) {
    switch (menu) {
    case MENU_RES_800x600: {
      // Engine_ResizeWindow(800, 600);
      break;
    }
    case MENU_RES_1000x800: {
      // Engine_ResizeWindow(1000, 800);
      break;
    }
    case MENU_RES_1280x720: {
      // Engine_ResizeWindow(1280, 720);
      break;
    }
    case MENU_RES_FULLSCREEN: {
      // Engine_SetWindowFullscreen();
      break;
    }
    default:
      // Engine_ResizeWindow(800, 600);
    }
  }
}