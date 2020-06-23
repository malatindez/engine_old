#ifndef MESH_H
#define MESH_H
#include "ShaderProgram.h"
#include "TextureManager.h"
class Mesh {
public:
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
private:
    //  render data
    unsigned int VAO = -1, VBO = -1, EBO = -1;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            &indices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
    // copy constructor
    Mesh(const Mesh& other) noexcept { }

    // move constructor
    Mesh(Mesh&& other) noexcept {  }

    // copy assignment
    Mesh& operator=(const Mesh& other) noexcept { }

    // move assignment
    Mesh& operator=(Mesh&& other) noexcept { }
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }
    ~Mesh() {
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void Draw(std::shared_ptr<ShaderProgram> shader) const noexcept {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            if (textures[i]->type == Texture::Type::DIFFUSE) {
                shader->setInt(("material.diffuse[" + std::to_string(diffuseNr++) + "]").c_str(), i);
            }
            else if (textures[i]->type == Texture::Type::SPECULAR) {
                shader->setInt(("material.specular[" + std::to_string(specularNr++) + "]").c_str(), i);
            }
            glBindTexture(GL_TEXTURE_2D, textures[i]->getID());
        }
        glActiveTexture(GL_TEXTURE0);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

};	
#endif