
#include "engine/client/render/Renderer.h"
#include "engine/client/render/Mesh.h"
#include "engine/Core.h"
namespace content::render {
class FractalRenderer : public engine::client::render::Renderer {
 public:

  FractalRenderer() {
    using engine::client::render::Mesh;
    std::vector<Mesh::Vertex> vertices = {
        Mesh::Vertex(glm::vec3(-1, -1, -1), glm::vec2(0, 0)),
        Mesh::Vertex(glm::vec3(1, -1, -1), glm::vec2(1, 0)),
        Mesh::Vertex(glm::vec3(1, 1, -1), glm::vec2(1, 1)),
        Mesh::Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 1)),
        Mesh::Vertex(glm::vec3(-1, -1, 1), glm::vec2(0, 0)),
        Mesh::Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 0)),
        Mesh::Vertex(glm::vec3(1, 1, 1), glm::vec2(1, 1)),
        Mesh::Vertex(glm::vec3(-1, 1, 1), glm::vec2(0, 1)),
    };
    std::vector<unsigned int> indices = {0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6,
                                         5, 4, 6, 6, 4, 7, 4, 0, 7, 7, 0, 3,
                                         3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};
    auto vertices_ptr = std::make_shared<std::vector<Mesh::Vertex>>(vertices);
    auto indices_ptr = std::make_shared<std::vector<unsigned int>>(indices);
    mesh_ = std::make_shared<Mesh>(vertices_ptr, indices_ptr);
    fractal_shader_ = engine::core::Core::GetInstance()->LoadShader("engine\\content\\shaders\\triangle.vert", "engine\\content\\shaders\\triangle.frag");
  }
  std::weak_ptr<engine::client::render::Shader> shader()
      const noexcept override {
    return fractal_shader_;
  }

  void Draw(std::weak_ptr<engine::core::Object> object) override {
    fractal_shader_->SetMat4("model", object.lock().get()->model_matrix());
    mesh_->Draw(fractal_shader_);
  }

 private:
  std::shared_ptr<engine::client::render::Mesh> mesh_;
  std::shared_ptr<engine::client::render::Shader> fractal_shader_;
};
}  // namespace content::render