#pragma once
#include "Mesh.h"

namespace engine::render {
class Model {
 public:
  Model(std::string path, TextureManager* const mngr) : texture_manager_(mngr) {
    loadModel(path);
  }
  Model(std::vector<std::shared_ptr<Mesh>> meshes, TextureManager* const mngr)
      : texture_manager_(mngr) {
    this->meshes_ = meshes;
  }

  void Draw(std::shared_ptr<ShaderProgram> shader) const noexcept {
    for (auto itr = meshes_.begin(); itr != meshes_.end(); itr++) {
      (*itr)->Draw(shader);
    }
  }
  void loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene* scene =
        import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
#ifdef CERR_OUTPUT
      std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
#endif
      return;
    }
    directory_ = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);
  }

  TextureManager* const texture_manager_;

 protected:
  struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    float shininess;
  };
  // copy constructor
  Model(const Model&) noexcept : texture_manager_(nullptr) {}

  // move constructor
  Model(Model&&) noexcept : texture_manager_(nullptr) {}

  // copy assignment
  Model& operator=(const Model&) noexcept {}

  // move assignment
  Model& operator=(Model&&) noexcept {}

  void ProcessNode(aiNode* node, const aiScene* scene) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      meshes_.push_back(ProcessMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      ProcessNode(node->mChildren[i], scene);
    }
  }

  Material LoadMaterial(aiMaterial* mat) {
    Material material;
    aiColor3D color(0.f, 0.f, 0.f);
    float shininess;

    mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    material.diffuse = glm::vec3(color.r, color.b, color.g);

    mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
    material.ambient = glm::vec3(color.r, color.b, color.g);

    mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
    material.specular = glm::vec3(color.r, color.b, color.g);

    mat->Get(AI_MATKEY_SHININESS, shininess);
    material.shininess = shininess;

    return material;
  }

  std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    std::shared_ptr<std::vector<Mesh::Vertex>> vertices(
        new std::vector<Mesh::Vertex>);
    std::shared_ptr<std::vector<unsigned int>> indices(
        new std::vector<unsigned int>);
    std::vector<std::shared_ptr<Texture>> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Mesh::Vertex vertex;
      // process vertex positions, normals and texture coordinates
      glm::vec3 vector;
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertex.position = vector;
      if (mesh->mTextureCoords[0]) {  // does the mesh contain texture
                                      // coordinates?
        glm::vec2 vec;
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.texCoords = vec;
      } else {
        vertex.texCoords = glm::vec2(0.0f);
      }
      vertices->push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
        indices->push_back(face.mIndices[j]);
      }
    }
    // process material
    if (mesh->mMaterialIndex >= 0) {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
      std::vector<std::shared_ptr<Texture>> z;
// ENGINE_MODEL_H_LOAD_TEXTURES_FROM_MATERIAL
#define ENGINE_MODEL_H_LTFM(x, y)           \
  z = LoadMaterialTextures(material, x, y); \
  textures.insert(textures.end(), z.begin(), z.end());
      ENGINE_MODEL_H_LTFM(aiTextureType_NONE, Texture::Type::kDiffuse);
      ENGINE_MODEL_H_LTFM(aiTextureType_UNKNOWN, Texture::Type::kDiffuse);
      ENGINE_MODEL_H_LTFM(aiTextureType_DIFFUSE, Texture::Type::kDiffuse);
      ENGINE_MODEL_H_LTFM(aiTextureType_SPECULAR, Texture::Type::kSpecular);
      ENGINE_MODEL_H_LTFM(aiTextureType_AMBIENT, Texture::Type::kAmbient);
      ENGINE_MODEL_H_LTFM(aiTextureType_EMISSIVE, Texture::Type::kEmissive);
      ENGINE_MODEL_H_LTFM(aiTextureType_HEIGHT, Texture::Type::kHeight);
      ENGINE_MODEL_H_LTFM(aiTextureType_NORMALS, Texture::Type::kNormals);
      ENGINE_MODEL_H_LTFM(aiTextureType_SHININESS, Texture::Type::kShininess);
      ENGINE_MODEL_H_LTFM(aiTextureType_OPACITY, Texture::Type::kOpacity);
      ENGINE_MODEL_H_LTFM(aiTextureType_DISPLACEMENT,
                          Texture::Type::kDisplacement);
      ENGINE_MODEL_H_LTFM(aiTextureType_LIGHTMAP, Texture::Type::kLightmap);
      ENGINE_MODEL_H_LTFM(aiTextureType_REFLECTION, Texture::Type::kReflection);
      ENGINE_MODEL_H_LTFM(aiTextureType_BASE_COLOR, Texture::Type::kBaseColor);
      ENGINE_MODEL_H_LTFM(aiTextureType_NORMAL_CAMERA,
                          Texture::Type::kNormalCamera);
      ENGINE_MODEL_H_LTFM(aiTextureType_EMISSION_COLOR,
                          Texture::Type::kEmissionColor);
      ENGINE_MODEL_H_LTFM(aiTextureType_METALNESS, Texture::Type::kMetalness);
      ENGINE_MODEL_H_LTFM(aiTextureType_DIFFUSE_ROUGHNESS,
                          Texture::Type::kDiffuseRoughness);
      ENGINE_MODEL_H_LTFM(aiTextureType_AMBIENT_OCCLUSION,
                          Texture::Type::kAmbientOcclusion);
    }
#undef ENGINE_MODEL_H_LTFM
    return std::shared_ptr<Mesh>(new Mesh(vertices, indices, textures));
  }

  std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(
      aiMaterial* mat, aiTextureType type, Texture::Type texture_type) {
    std::vector<std::shared_ptr<Texture>> textures;
    typedef Texture::glTexParameterValues tParam;
    std::vector<tParam> v;
    v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                       GL_NEAREST_MIPMAP_NEAREST));
    v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
      aiString str;
      mat->GetTexture(type, i, &str);
      std::shared_ptr<Texture> texture =
          texture_manager_->Load(std::string(str.C_Str()), v, texture_type);
      textures.push_back(texture);
    }
    return textures;
  }

  // model data
  std::vector<std::shared_ptr<Mesh>> meshes_;
  std::string directory_;
};
}  // namespace engine::render