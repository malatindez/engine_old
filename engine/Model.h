#ifndef MODEL_H
#define MODEL_H
#include "Mesh.h"

class Model {
    struct Material {
        glm::vec3 Diffuse;
        glm::vec3 Specular;
        glm::vec3 Ambient;
        float Shininess;
    };
    // copy constructor
    Model(const Model& other) noexcept : textureManager(nullptr) { }

    // move constructor
    Model(Model&& other) noexcept : textureManager(nullptr) {  }

    // copy assignment
    Model& operator=(const Model& other) noexcept { }

    // move assignment
    Model& operator=(Model&& other) noexcept { }

    // model data
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::string directory;

    void processNode(aiNode* node, const aiScene* scene) {
        // process all the node's meshes (if any)
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }
    
    Material loadMaterial(aiMaterial* mat) {
        Material material;
        aiColor3D color(0.f, 0.f, 0.f);
        float shininess;

        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material.Diffuse = glm::vec3(color.r, color.b, color.g);

        mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
        material.Ambient = glm::vec3(color.r, color.b, color.g);

        mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material.Specular = glm::vec3(color.r, color.b, color.g);

        mat->Get(AI_MATKEY_SHININESS, shininess);
        material.Shininess = shininess;

        return material;
    }

    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Mesh::Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<std::shared_ptr<Texture>> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Mesh::Vertex vertex;
            // process vertex positions, normals and texture coordinates
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            }
            else {
                vertex.texCoords = glm::vec2(0.0f);
            }
            vertices.push_back(vertex);
        }
        // process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        // process material
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<std::shared_ptr<Texture>> z;
#define load_textures_from_material(x,y) \
z = loadMaterialTextures(material, x, y); \
textures.insert(textures.end(), z.begin(), z.end());
            load_textures_from_material(aiTextureType_NONE, Texture::Type::DIFFUSE);
            load_textures_from_material(aiTextureType_UNKNOWN, Texture::Type::DIFFUSE);
            load_textures_from_material(aiTextureType_DIFFUSE, Texture::Type::DIFFUSE);
            load_textures_from_material(aiTextureType_SPECULAR, Texture::Type::SPECULAR);
            load_textures_from_material(aiTextureType_AMBIENT, Texture::Type::AMBIENT);
            load_textures_from_material(aiTextureType_EMISSIVE, Texture::Type::EMISSIVE);
            load_textures_from_material(aiTextureType_HEIGHT, Texture::Type::HEIGHT);
            load_textures_from_material(aiTextureType_NORMALS, Texture::Type::NORMALS);
            load_textures_from_material(aiTextureType_SHININESS, Texture::Type::SHININESS);
            load_textures_from_material(aiTextureType_OPACITY, Texture::Type::OPACITY);
            load_textures_from_material(aiTextureType_DISPLACEMENT, Texture::Type::DISPLACEMENT);
            load_textures_from_material(aiTextureType_LIGHTMAP, Texture::Type::LIGHTMAP);
            load_textures_from_material(aiTextureType_REFLECTION, Texture::Type::REFLECTION);
            load_textures_from_material(aiTextureType_BASE_COLOR, Texture::Type::BASE_COLOR);
            load_textures_from_material(aiTextureType_NORMAL_CAMERA, Texture::Type::NORMAL_CAMERA);
            load_textures_from_material(aiTextureType_EMISSION_COLOR, Texture::Type::EMISSION_COLOR);
            load_textures_from_material(aiTextureType_METALNESS, Texture::Type::METALNESS);
            load_textures_from_material(aiTextureType_DIFFUSE_ROUGHNESS, Texture::Type::DIFFUSE_ROUGHNESS);
            load_textures_from_material(aiTextureType_AMBIENT_OCCLUSION, Texture::Type::AMBIENT_OCCLUSION);
        }

        return std::shared_ptr<Mesh>(new Mesh(std::shared_ptr<std::vector<Mesh::Vertex>>(&vertices), std::shared_ptr<std::vector<unsigned int>>(&indices), textures));
    }
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::Type textureType) {
        std::vector<std::shared_ptr<Texture>> textures;
        typedef Texture::glTexParameterValues tParam;
        std::vector<tParam> v;
        v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
        v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::shared_ptr<Texture> texture = textureManager->Load(std::string(str.C_Str()), v, textureType);
            textures.push_back(texture);
        }
        return textures;
    }
public:
    TextureManager *const textureManager;
    Model(std::string path, TextureManager* const mngr) : textureManager(mngr) {
        loadModel(path);
    }
    Model(std::vector<std::shared_ptr<Mesh>> meshes, TextureManager* const mngr) : textureManager(mngr) {
        this->meshes = meshes;
    }
    void Draw(std::shared_ptr<ShaderProgram> shader) const noexcept {
        for (auto itr = meshes.begin(); itr != meshes.end(); itr++) {
            (*itr)->Draw(shader);
        }
    }

    void loadModel(std::string path) {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
#ifdef CERR_OUTPUT
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
#endif
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }
};
#endif