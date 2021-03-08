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

#include <engine/client/Player.h>
#include <engine/client/misc/Window.h>
#include <engine/client/render/Camera.h>
#include <engine/client/render/Mesh.h>

#include "content/code/Objects/Fractal.h"
#include "engine/Core.h"

/*
#ifdef WIN32
int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow) {
#else*/
int main() {
//#endif
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto window = std::make_shared<engine::client::Window>(
      1366, 768, "engine " + std::string(ENGINE_VERSION), nullptr, nullptr);
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


  auto core = engine::core::Core::GetInstance();
  core->AddTickingObject(window);
  engine::client::Player player(window, glm::vec3(0, 1, 0));

  window->SwapInterval(1);
  using engine::client::Window;
  using engine::client::render::Shader;
  using content::render::FractalRenderer;

  auto f = std::make_shared<content::objects::Fractal>();
  auto renderer = std::dynamic_pointer_cast<FractalRenderer>(f->renderer());
  auto shader = renderer->shader();

  f->SetPosition(glm::vec3(0, 0, 1));

      

  std::string vertex_code = "";
  std::string fragment_code = "";

  auto shader_update_lambda = [&] {
    std::string a = Shader::LoadSourceCode(
        "content\\shaders\\triangle.vert");
    std::string b = Shader::LoadSourceCode(
        "content\\shaders\\triangle.frag");
    if (a != vertex_code || b != fragment_code) {
      vertex_code = a;
      fragment_code = b;
      auto source = engine::client::render::Shader::ShaderSource(a, b);
      auto shader_ = std::make_shared<engine::client::render::Shader>(source);
      renderer->SetShader(shader_);
      shader = shader_;
    }
  };

  while (!window->ShouldClose()) {
    shader_update_lambda();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.1F, 0.1F, 0.15F, 1.0F);
    glm::mat4 matrix = glm::perspective(glm::radians(player.camera()->FOV()),
                                        (float)window->GetWindowSize().x /
                                            (float)window->GetWindowSize().y,
                                        0.0000001F, 100.0F) *
                       player.camera()->view_matrix();
    shader.lock()->Use();
    shader.lock()->SetMat4("fullMatrix", matrix);
    shader.lock()->SetFloat("time", (float)glfwGetTime());
    renderer->Draw(f);
    window->SwapBuffers();
    window->PollEvents();
    double t = abs(player.position().z -  f->position().z);
    double u = log1p(t);
    player.SetVelocity((float)u);
  }
}
/*
uniform mat4 model;
uniform mat4 fullMatrix;
uniform sampler2D normals;
uniform vec3 viewPos;
*/