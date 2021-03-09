#pragma once
#include <stdint.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "stb_image.h"


namespace engine::client::render {
class Texture {
 public:

  Texture(unsigned char const* data, int width, int height, int channels,
          std::string const& path = "")
      : path_(path) {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    if (data != nullptr) {
      GLenum format = GL_RED;
      if (channels == 1) {
        format = GL_RED;
      } else if (channels == 3) {
        format = GL_RGB;
      } else if (channels == 4) {
        format = GL_RGBA;
      }
      glBindTexture(GL_TEXTURE_2D, id_);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

    }
  }

  ~Texture() { glDeleteTextures(1, &id_); }

  [[nodiscard]] uint32_t id() const noexcept { return id_; }
  [[nodiscard]] std::string path() const noexcept { return path_; }

  
 private:
  Texture(const Texture&) = delete;
  Texture(Texture&&) = delete;
  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&&) = delete;


  uint32_t id_ = 0;
  std::string path_;
};
}  // namespace engine::client::render