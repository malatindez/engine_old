
#include "engine/client/render/Renderer.h"
#include "engine/client/render/Mesh.h"
#include "engine/Core.h"
namespace content::render {
class FractalRenderer : public engine::client::render::Renderer {
 public:

  FractalRenderer() {
    using engine::client::render::Mesh;
    std::vector<Mesh::Vertex> vertices = {
        Mesh::Vertex(glm::vec3(0.5, 0.5, 0), glm::vec2(1, 1)),
        Mesh::Vertex(glm::vec3(0.5, -0.5, 0), glm::vec2(1, 0)),
        Mesh::Vertex(glm::vec3(-0.5, -0.5, 0), glm::vec2(0, 0)),
        Mesh::Vertex(glm::vec3(-0.5, 0.5, 0), glm::vec2(0, 1)),
    };
    std::vector<unsigned int> indices = {0, 1, 3, 1,2,3};
    vertices_ = std::make_shared<std::vector<Mesh::Vertex>>();
    indices_ = std::make_shared<std::vector<unsigned int>>();
    vertices_->assign(vertices.begin(), vertices.end());
    indices_->assign(indices.begin(), indices.end());
    mesh_ = std::make_shared<Mesh>(vertices_, indices_);
    fractal_shader_ = engine::core::Core::GetInstance()->LoadShader(
        "F:\\cpp\\MVS\\engine\\engine\\content\\shaders\\triangle.vert",
        "F:\\cpp\\MVS\\engine\\engine\\content\\shaders\\triangle.frag");
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

  std::shared_ptr<std::vector<engine::client::render::Mesh::Vertex>> vertices_;
  std::shared_ptr<std::vector<unsigned int>> indices_;
};
}  // namespace content::render