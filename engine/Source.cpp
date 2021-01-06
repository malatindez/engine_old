
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION


#include "core/Core.h"
#include "render/misc/Camera.h"

bool function(int32_t, int32_t) {
  return false;
}
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto window = std::make_shared<engine::core::Window>(1920, 1080, "engine",
                                                       nullptr, nullptr);
  window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  using namespace std::placeholders;
  auto wKeybindPtr =
      std::make_shared<engine::core::Input::KeyBindCallback>(std::bind(&function, _1, _2));
  window->AddKeyCallback(glfwGetKeyScancode(GLFW_KEY_W), wKeybindPtr);
  window->MakeContextCurrent();

  if (!window->Alive()) {
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
  engine::render::Camera cam(window, glm::vec3(0,1,0));
  while (!window->ShouldClose()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.1F, 0.1F, 0.15F, 1.0F);
    window->SwapBuffers();
    window->PollEvents();
    window->Update(0,0);
  }
}