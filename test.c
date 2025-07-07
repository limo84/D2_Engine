#include "src/d2.h"

void mainloop() {
}

int main() {
  Engine_Init();
  Engine_OpenWindow(640, 480, true);
  Engine_RunMainloop(mainloop);
  return 0;
}
