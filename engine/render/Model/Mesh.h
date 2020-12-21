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

 private:
  //  render data
  unsigned int VAO = 0xffffffff, VBO = 0xffffffff, EBO = 0xffffffff;
  unsigned int indices_size{};
  void setupMesh(std::shared_ptr<std::vector<Vertex>> vertices,
                 std::shared_ptr<std::vector<unsigned int>> indices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex),
                 &(*vertices)[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
    this->indices_size = (unsigned int)indices->size();
  }
  // no copy neither move construtors/assignments allowed

  // move constructor
  Mesh(Mesh&&) noexcept {}

  // move assignment
  Mesh& operator=(Mesh&&) noexcept {}

  // copy constructor
  Mesh(const Mesh&) noexcept {}
  // copy assignment
  Mesh& operator=(const Mesh&) noexcept {}

 public:
  // mesh data
  std::vector<std::shared_ptr<Texture>> textures;

  Mesh(std::shared_ptr<std::vector<Vertex>> vertices,
       std::shared_ptr<std::vector<unsigned int>> indices,
       std::vector<std::shared_ptr<Texture>> textures) {
    this->textures = textures;

    setupMesh(vertices, indices);
  }
  ~Mesh() {
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
  }

  void Draw(std::shared_ptr<ShaderProgram> shader) const noexcept {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    for (unsigned int i = 0; i < textures.size(); i++) {
      glActiveTexture(GL_TEXTURE0 +
                      i);  // activate proper texture unit before binding
      // retrieve texture number (the N in diffuse_textureN)
      std::string number;
      if (textures[i]->type == Texture::Type::DIFFUSE) {
        shader->setInt(
            ("material.diffuseTextures[" + std::to_string(diffuseNr++) + "]"),
            i);
      } else if (textures[i]->type == Texture::Type::SPECULAR) {
        shader->setInt(
            ("material.specularTextures[" + std::to_string(specularNr++) + "]"),
            i);
      } else if (textures[i]->type == Texture::Type::NORMALS) {
        shader->setInt("normal", i);
      } else if (textures[i]->type == Texture::Type::HEIGHT) {
        shader->setInt(
            "material.diffuseTextures[" + std::to_string(diffuseNr++) + "]", i);
      }
      glBindTexture(GL_TEXTURE_2D, textures[i]->getID());
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
};
#endif
#pragma warning(default : 26495)