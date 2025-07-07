#include "d2.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef void *game_run_func(void);

void *module_main_function = NULL;

int main(void) {
  
  Engine_Init();
  Engine_OpenWindow(1600, 900, false);

  while (!glfwWindowShouldClose(Engine_GetEngine()->window)) {
    
    // system("make libd2.so");
    system("make libgame.so");

    void *gameModule = dlopen("libgame/libgame.so", RTLD_NOW);
    if (!gameModule) {
      printf("%s\n", dlerror());
    }
    //   module_init_function = dlsym(module, "Game_Init");
    game_run_func *game_run = dlsym(gameModule, "Game_RunGame");
    printf("Game Run Func: %p\n", game_run);

    // module_main_function = dlsym(module, "MainloopFunction");
    Engine_GetEngine()->isRunning = true;
    game_run();
    // Engine_RunMainloop(module_main_function);
    dlclose(gameModule);
  }
  glfwTerminate();
  return 0;
}
