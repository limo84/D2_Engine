// #include "d2_structs.h"
#ifdef USE_GLFW

#include "d2.h"
#include "d2_priv.h"
#include <GLFW_3_4/glfw3.h>

struct Engine {
  GLFWwindow *window;
  // GLFWevent event;
  bool isRunning;
};

Engine *engine;

extern bool keys[512];
extern bool oldKeys[512];
extern Mouse mouse;
extern Mouse oldMouse;

double startTime, endTime;
double timeSinceLastFpsPrint = 0;
u16 deltaInMillis = 0;
u64 frames = 0;

bool vsyncEnabled = 1; // TODO
u8 mon = 1;            // TODO

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void _Backend_CreateWindow(u32 width, u32 height, bool fullscreen) {

  engine = malloc(sizeof(Engine));
  engine->isRunning = true;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_AUTO_ICONIFY, 0);

  int count;
  GLFWmonitor **monitors = glfwGetMonitors(&count);
  GLFWmonitor *primary = glfwGetPrimaryMonitor();
  printf("monitor count: %d", count);

  GLFWmonitor *monitor = NULL;
  if (fullscreen) {
    monitor = monitors[mon]; // TODO
  } else {
    int xpos, ypos;
    glfwGetMonitorPos(monitors[mon], &xpos, &ypos);
    const GLFWvidmode *mode = glfwGetVideoMode(monitors[mon]);
    glfwWindowHint(GLFW_POSITION_X, xpos + mode->width / 2 - width / 2);
    glfwWindowHint(GLFW_POSITION_Y, ypos + mode->height / 2 - height / 2);
  }

  GLFWwindow *window = glfwCreateWindow(width, height, "LearnOpenGL", monitor, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(vsyncEnabled ? 1 : 0);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    exit(-1);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  engine->window = window;
  // return window;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    keys[key] = true;
  }

  if (action == GLFW_RELEASE) {
    keys[key] = false;
  }
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  mouse.x = xpos;
  mouse.y = Engine_GetWindowHeight() - ypos;
  // printf("mouse: x->%f, y%f\n", xpos, mouse.y);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  // if (action == GLFW_MOUS)
  if (action == GLFW_PRESS) {
    mouse.button[button] = true;
    printf("button pressed: %d ___ mouse: x->%f, y%f\n", button, mouse.x, mouse.y);
  }

  if (action == GLFW_RELEASE) {
    mouse.button[button] = false;
  }
}

void _Backend_SwapBuffers() { glfwSwapBuffers(engine->window); }

bool Engine_GetIsRunning() { return engine->isRunning; }

void Engine_SetIsRunning(bool setRunning) { engine->isRunning = setRunning; }

Engine *Engine_GetEngine() { return engine; }

void _Backend_CalculateDelta() {
  endTime = glfwGetTime();
  float deltaInSeconds = endTime - startTime;
  deltaInMillis = deltaInSeconds * 1000;
  startTime = endTime;

  frames++;
  timeSinceLastFpsPrint += deltaInSeconds;
  if (timeSinceLastFpsPrint >= 1) {
    // printf("FPS: %f\n", frames / timeSinceLastFpsPrint);
    timeSinceLastFpsPrint = 0;
    frames = 0;
  }
}

void _Backend_InitTime() {
  startTime = glfwGetTime();
  endTime = glfwGetTime();
}

u16 Engine_GetDelta() { return deltaInMillis; }

void _Backend_HandleEvents() {

  glfwPollEvents();

  if (Engine_GetKeyPressedOnce(GLFW_KEY_F5)) {
    Engine_SetIsRunning(false);
  }
  // while (SDL_PollEvent(&(engine->event))) {

  //   switch (engine->event.type) {

  //   case SDL_WINDOWEVENT: {
  //     if (engine->event.window.event == SDL_WINDOWEVENT_RESIZED) {
  //       // printf("w: %d, h: %d\n", window_width, window_height);
  //       // Mat4_print("proj", m.projection);
  //       window_width = engine->event.window.data1;
  //       window_height = engine->event.window.data2;
  //       glViewport(0, 0, window_width, window_height);
  //       _Engine_UpdateProjectionMatrix();
  //       globalScale = window_width / 800.0;
  //       // Mat4_print("proj", m.projection);
  //       // printf("w: %d, h: %d\n", window_width, window_height);
  //       // Mat4_print("VP", m.vp);
  //     }
  //   } break;

  //   case SDL_QUIT: {
  //     engine->isRunning = 0;
  //   } break;

  //   case SDL_MOUSEMOTION: {
  //     mouse.x = engine->event.button.x;
  //     mouse.y = window_height - engine->event.button.y;
  //   } break;

  //   case SDL_MOUSEBUTTONDOWN: {
  //     mouse.button[engine->event.button.button] = true;
  //     mouse.x = engine->event.button.x;
  //     mouse.y = window_height - engine->event.button.y;
  //     printf("mouse down: %d,%d\n", engine->event.button.button,
  //            engine->event.button.y);
  //   } break;

  //   case SDL_MOUSEBUTTONUP: {
  //     mouse.button[engine->event.button.button] = false;
  //     printf("mouse up: %d,%d\n", engine->event.button.button,
  //            engine->event.button.y);
  //   } break;

  //   case SDL_KEYDOWN: {
  //     SDL_Scancode code = engine->event.key.keysym.scancode;
  //     keys[code] = true;
  //   } break;

  //   case SDL_KEYUP: {
  //     SDL_Scancode code = engine->event.key.keysym.scancode;
  //     keys[code] = false;
  //   } break;
  //   }
  // }
}

#endif
