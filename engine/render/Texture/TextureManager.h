#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <memory>
#include <unordered_map>

#include "../../header_libs/hash/xxh3.h"
#include "FrameTicker.h"
#include "Texture.h"
class TextureManager : public FrameTicker {
 public:
  TextureManager() : FrameTicker(4096) {}
  virtual void Update(const unsigned int tick, const float) noexcept {
    if (tick % getTickRate() == 0) {
      std::vector<XXH32_hash_t> valuesToErase;
      for (auto kv : textures_) {
        if (kv.second.unique()) {
          valuesToErase.push_back(kv.first);
        }
      }
      for (auto itr = valuesToErase.begin(); itr != valuesToErase.end();
           itr++) {
        textures_.erase((*itr));
      }
    }
  }
  std::shared_ptr<Texture> Load(
      std::string path, std::vector<Texture::glTexParameterValues> values,
      Texture::Type type) {
    int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load(true);

    unsigned char* data =
        stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
      throw std::exception("path_not_found");
    }
    try {
      return Load(data, width, height, nrChannels, values, type, path);
    } catch (std::exception e) {
      stbi_image_free(data);
      throw e;
    }
    stbi_image_free(data);
  }

  std::shared_ptr<Texture> Load(
      unsigned char* data, int width, int height, int channels,
      std::vector<Texture::glTexParameterValues> values, Texture::Type type,
      std::string path = "") {
    XXH32_hash_t hash =
        CalculateTextureHash(data, width, height, channels, values);
    if (textures_.find(hash) != textures_.end()) {
      return textures_.at(hash);
    }
    std::shared_ptr<Texture> returnValue(
        new Texture(data, width, height, channels, values, type, path));
    textures_[hash] = returnValue;
    return returnValue;
  }

 protected:
  std::unordered_map<XXH32_hash_t, std::shared_ptr<Texture>> textures_;

 private:
  static XXH32_hash_t CalculateTextureHash(
      unsigned char* data, int width, int height, int channels,
      std::vector<Texture::glTexParameterValues> values) {
    XXH32_state_t* const state = XXH32_createState();
    if (state == nullptr) {
      throw std::exception("failed_to_create_hash_state");
    }

    XXH64_hash_t const seed = 0; /* or any other value */
    if (XXH32_reset(state, seed) == XXH_ERROR) {
      throw std::exception("failed_to_initialize_hash_state");
    }
#pragma warning(disable : 26451)
    if (XXH32_update(state, data, (channels * width * height)) == XXH_ERROR) {
      throw std::exception("failed_to_update_hash");
    }
#pragma warning(disable : 26451)
    for (auto itr = values.begin(); itr != values.end(); itr++) {
      if (XXH32_update(state, &(*itr).target, sizeof(GLenum)) == XXH_ERROR) {
        throw std::exception("failed_to_update_hash");
      }
      if (XXH32_update(state, &(*itr).pname, sizeof(GLenum)) == XXH_ERROR) {
        throw std::exception("failed_to_update_hash");
      }
      if (XXH32_update(state, &(*itr).type,
                       sizeof(Texture::glTexParameterType)) == XXH_ERROR) {
        throw std::exception("failed_to_update_hash");
      }
      int size = 0;
      switch ((*itr).type) {
        case Texture::glTexParameterType::kFloat:
          size = sizeof(GLfloat);
          break;
        case Texture::glTexParameterType::kInteger:
          size = sizeof(GLint);
          break;
        case Texture::glTexParameterType::kFloatVec:
          size = sizeof(glm::vec3);
          break;
        case Texture::glTexParameterType::kIIntVec:
        case Texture::glTexParameterType::kIntVec:
          size = sizeof(glm::ivec3);
          break;
        case Texture::glTexParameterType::kIUIntVec:
          size = sizeof(glm::uvec3);
          break;
      }
      if (XXH32_update(state, (*itr).ptr, size) == XXH_ERROR) {
        throw std::exception("failed_to_update_hash");
      }
    }

    XXH32_hash_t const hash = XXH32_digest(state);
    XXH32_freeState(state);
    return hash;
  }
};

#endif
