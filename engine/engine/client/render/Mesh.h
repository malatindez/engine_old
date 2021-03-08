#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "Shader.h"
#include "Texture.h"

namespace engine::client::render {
class Mesh {
 public:
  struct Vertex {
    glm::vec3 position;
    glm::vec2 tex_coords;
    Vertex(glm::vec3 const& pos, glm::vec2 const& tex_coords)
        : position(pos), tex_coords(tex_coords) {}
  };

  Mesh(std::shared_ptr<std::vector<Vertex>> vertices,
       std::shared_ptr<std::vector<unsigned int>> indices,
       std::vector<std::shared_ptr<Texture>> const& textures = {})
      : textures_(textures) {
    setupMesh(vertices, indices);
  }
  ~Mesh() {
    glDeleteBuffers(1, &EBO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteVertexArrays(1, &VAO_);
  }

  void Draw(std::shared_ptr<Shader> shader) const noexcept {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    for (unsigned int i = 0; i < textures_.size(); i++) {
      glActiveTexture(GL_TEXTURE0 +
                      i);  // activate proper texture unit before binding
      // retrieve texture number (the N in diffuse_textureN)

      glBindTexture(GL_TEXTURE_2D, textures_[i]->id());
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

 private:
  void setupMesh(std::shared_ptr<std::vector<Vertex>> vertices,
                 std::shared_ptr<std::vector<unsigned int>> indices) {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex),
                 &(*vertices)[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices->size() * sizeof(unsigned int), &(*indices)[0],
                 GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, tex_coords));

    glBindVertexArray(0);
    this->indices_size_ = indices->size();
  }
  // no copy neither move construtors/assignments allowed

  Mesh(const Mesh&) = delete;
  Mesh(Mesh&&) = delete;
  Mesh& operator=(const Mesh&) = delete;
  Mesh& operator=(Mesh&&) = delete;

  // mesh data
  std::vector<std::shared_ptr<Texture>> textures_;

  //  render data
  uint32_t VAO_ = -1;
  uint32_t VBO_ = -1;
  uint32_t EBO_ = -1;
  size_t indices_size_;
};
}  // namespace engine::client::render