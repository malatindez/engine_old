
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "core/core.h"
#include "header_libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#define SCR_RES SCR_WIDTH, SCR_HEIGHT
#include <iostream>
#include <functional>
bool function(int32_t scancode, int32_t action) {
  std::cout << scancode << " " << action << " "
            << "W PRESSED" << std::endl;
  return false;
}
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  engine::core::Window window(1920, 1080, "engine", NULL, NULL);
  using namespace std::placeholders;
  std::shared_ptr<engine::core::Input::KeyBindCallback> wKeybindPtr =
      std::make_shared<engine::core::Input::KeyBindCallback>(std::bind(&function, _1, _2));
  window.AddKeyCallback(glfwGetKeyScancode(GLFW_KEY_W), wKeybindPtr);
  window.MakeContextCurrent();

  if (!window.Alive()) {
#ifdef CERR_OUTPUT
    std::cerr << "Failed to create GLFW window" << std::endl;
#endif
    glfwTerminate();
    return -1;
  }
  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
#ifdef CERR_OUTPUT
    std::cerr << "Failed to initialize GLAD" << std::endl;
#endif
    return -1;
  }
  glEnable(GL_DEPTH_TEST);

  while (!window.ShouldClose()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    window.SwapBuffers();
    window.PollEvents();
    window.Update(0,0);
  }
}