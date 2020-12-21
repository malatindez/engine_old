#ifndef MESH_H
#define MESH_H
#include "../Shader/ShaderProgram.h"
#include "../Texture/TextureManager.h"
#pragma warning(disable : 26495)
class Mesh {
 public:
  struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoords;
  };

  Mesh(std::shared_ptr<std::vector<Vertex>> vertices,
       std::shared_ptr<std::vector<unsigned int>> indices,
       std::vector<std::shared_ptr<Texture>> textures) {
    this->textures_ = textures;

    SetupMesh(vertices, indices);
  }
  ~Mesh() {
    glDeleteBuffers(1, &EBO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteVertexArrays(1, &VAO_);
  }

  void Draw(std::shared_ptr<ShaderProgram> shader) const noexcept {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    for (unsigned int i = 0; i < textures_.size(); i++) {
      glActiveTexture(GL_TEXTURE0 +
                      i);  // activate proper texture unit before binding
      // retrieve texture number (the N in diffuse_textureN)
      std::string number;
      if (textures_[i]->type == Texture::Type::kDiffuse) {
        shader->SetInt(
            ("material.diffuseTextures[" + std::to_string(diffuseNr++) + "]"),
            i);
      } else if (textures_[i]->type == Texture::Type::kSpecular) {
        shader->SetInt(
            ("material.specularTextures[" + std::to_string(specularNr++) + "]"),
            i);
      } else if (textures_[i]->type == Texture::Type::kNormals) {
        shader->SetInt("normal", i);
      } else if (textures_[i]->type == Texture::Type::kHeight) {
        shader->SetInt(
            "material.diffuseTextures[" + std::to_string(diffuseNr++) + "]", i);
      }
      glBindTexture(GL_TEXTURE_2D, textures_[i]->GetID());
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  // mesh data
  std::vector<std::shared_ptr<Texture>> textures_;

 private:
  // no copy neither move construtors/assignments allowed

  // move constructor
  Mesh(Mesh&&) noexcept {}
  // move assignment
  Mesh& operator=(Mesh&&) noexcept {}
  // copy constructor
  Mesh(const Mesh&) noexcept {}
  // copy assignment
  Mesh& operator=(const Mesh&) noexcept {}

  void SetupMesh(std::shared_ptr<std::vector<Vertex>> vertices,
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
    this->indices_size_ = (unsigned int)indices->size();
  }

  // render data
  unsigned int VAO_ = 0xffffffff, VBO_ = 0xffffffff, EBO_ = 0xffffffff;
  unsigned int indices_size_;
};
#endif
#pragma warning(default : 26495)