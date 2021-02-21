#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Config.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include <functional>
#include <iostream>
#ifdef WIN32
#include <Windows.h>
#endif

#define ENGINE_CLIENT

#include <engine/client/Player.h>
#include <engine/client/misc/Window.h>
#include <engine/client/render/Camera.h>
#include <engine/client/render/Mesh.h>

#include "content/code/Objects/Fractal.h"
#include "engine/Core.h"

#ifdef WIN32
int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow) {
#else
int main() {
#endif

  int i = 0;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto window = std::make_shared<engine::client::Window>(
      1920, 1080, "engine " + std::string(ENGINE_VERSION), nullptr, nullptr);
  window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

  engine::client::Player player(window, glm::vec3(0, 1, 0));

  window->SwapInterval(0);

  auto f = std::make_shared<content::objects::Fractal>();
  auto shader = f->renderer()->shader();

  while (!window->ShouldClose()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.1F, 0.1F, 0.15F, 1.0F);
    window->SwapBuffers();
    window->PollEvents();
    window->UpdateExecutionTime(0);
    glm::mat4 matrix = glm::perspective(glm::radians(player.camera()->FOV()),
                                        (float)window->GetWindowSize().x /
                                            (float)window->GetWindowSize().y,
                                        0.0000001F, 100.0F) *
                       player.camera()->view_matrix();
    shader.lock()->SetMat4("fullMatrix", matrix);
    f->renderer()->Draw(f);
  }
}
/*
uniform mat4 model;
uniform mat4 fullMatrix;
uniform sampler2D normals;
uniform vec3 viewPos;
*/