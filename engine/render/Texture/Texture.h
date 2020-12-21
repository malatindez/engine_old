#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>

#include <string>
#include <vector>
#ifdef CERR_OUTPUT
#include <iostream>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../header_libs/stb_image.h"

// after creating this object you should bind it
// using glBindTexture(GL_TEXTURE_2D, obj.getID());
//
// and set parameters
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
// and set texture filtering parameters
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
class Texture {
 public:
  enum class Type {
    /** The texture is combined with the result of the diffuse
     *  lighting equation.
     */
    kDiffuse,
    /** The texture is combined with the result of the specular
     *  lighting equation.
     */
    kSpecular,
    /** The texture is combined with the result of the ambient
     *  lighting equation.
     */
    kAmbient,
    /** The texture is added to the result of the lighting
     *  calculation. It isn't influenced by incoming light.
     */
    kEmissive,
    /** The texture is a height map.
     *
     *  By convention, higher gray-scale values stand for
     *  higher elevations from the base height.
     */
    kHeight,
    /** The texture is a (tangent space) normal-map.
     *
     *  Again, there are several conventions for tangent-space
     *  normal maps. Assimp does (intentionally) not
     *  distinguish here.
     */
    kNormals,
    /** The texture defines the glossiness of the material.
     *
     *  The glossiness is in fact the exponent of the specular
     *  (phong) lighting equation. Usually there is a conversion
     *  function defined to map the linear color values in the
     *  texture to a suitable exponent. Have fun.
     */
    kShininess,
    /** The texture defines per-pixel opacity.
     *
     *  Usually 'white' means opaque and 'black' means
     *  'transparency'. Or quite the opposite. Have fun.
     */
    kOpacity,
    /** Displacement texture
     *
     *  The exact purpose and format is application-dependent.
     *  Higher color values stand for higher vertex displacements.
     */
    kDisplacement,
    /** Lightmap texture (aka Ambient Occlusion)
     *
     *  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
     *  covered by this material property. The texture contains a
     *  scaling value for the final color value of a pixel. Its
     *  intensity is not affected by incoming light.
     */
    kLightmap,
    /** Reflection texture
     *
     * Contains the color of a perfect mirror reflection.
     * Rarely used, almost never for real-time applications.
     */
    kReflection,
    /** PBR Materials
     * PBR definitions from maya and other modelling packages now use this
     * standard. This was originally introduced around 2012. Support for this is
     * in game engines like Godot, Unreal or Unity3D. Modelling packages which
     * use this are very common now.
     */
    kBaseColor,
    kNormalCamera,
    kEmissionColor,
    kMetalness,
    kDiffuseRoughness,
    kAmbientOcclusion
  } type;
  enum class glTexParameterType {
    kFloat,
    kInteger,
    kFloatVec,
    kIIntVec,
    kIntVec,
    kIUIntVec
  };
  struct glTexParameterValues {
    // copy constructor
    glTexParameterValues(const glTexParameterValues& other) {
      switch (other.type) {
        case glTexParameterType::kFloat:
          ptr = new GLfloat((*(GLfloat*)other.ptr));
          break;
        case glTexParameterType::kInteger:
          ptr = new GLint((*(GLint*)other.ptr));
          break;
        case glTexParameterType::kFloatVec:
          ptr = new glm::vec3((*(glm::vec3*)other.ptr));
          break;
        case glTexParameterType::kIIntVec:
        case glTexParameterType::kIntVec:
          ptr = new glm::ivec3((*(glm::ivec3*)other.ptr));
          break;
        case glTexParameterType::kIUIntVec:
          ptr = new glm::uvec3((*(glm::uvec3*)other.ptr));
          break;
      }
      type = other.type;
      target = other.target;
      pname = other.pname;
    }

    // move constructor
    glTexParameterValues(glTexParameterValues&& other) noexcept {
      type = other.type;
      target = other.target;
      pname = other.pname;
      ptr = other.ptr;
      other.ptr = nullptr;
    }

    // copy assignment
    glTexParameterValues& operator=(const glTexParameterValues& other) {
      switch (other.type) {
        case glTexParameterType::kFloat:
          ptr = new GLfloat((*(GLfloat*)other.ptr));
          break;
        case glTexParameterType::kInteger:
          ptr = new GLint((*(GLint*)other.ptr));
          break;
        case glTexParameterType::kFloatVec:
          ptr = new glm::vec3((*(glm::vec3*)other.ptr));
          break;
        case glTexParameterType::kIIntVec:
        case glTexParameterType::kIntVec:
          ptr = new glm::ivec3((*(glm::ivec3*)other.ptr));
          break;
        case glTexParameterType::kIUIntVec:
          ptr = new glm::uvec3((*(glm::uvec3*)other.ptr));
          break;
      }
      type = other.type;
      target = other.target;
      pname = other.pname;
      return *this;
    }

    // move assignment
    glTexParameterValues& operator=(glTexParameterValues&& other) noexcept {
      type = other.type;
      target = other.target;
      pname = other.pname;
      ptr = other.ptr;
      other.ptr = nullptr;
      return *this;
    }
    ~glTexParameterValues() {
      if (ptr != nullptr) {
        switch (type) {
          case glTexParameterType::kFloat:
            delete (GLfloat*)(ptr);
            break;
          case glTexParameterType::kInteger:
            delete (GLint*)(ptr);
            break;
          case glTexParameterType::kFloatVec:
            delete (glm::vec3*)(ptr);
            break;
          case glTexParameterType::kIIntVec:
          case glTexParameterType::kIntVec:
            delete (glm::ivec3*)(ptr);
            break;
          case glTexParameterType::kIUIntVec:
            delete (glm::uvec3*)(ptr);
            break;
        }
      }
    }

    glTexParameterType type;
    GLenum target, pname;
    void* ptr;  // pointer to memory where is our *type* value stored

    glTexParameterValues(GLenum target, GLenum pname, GLfloat param) noexcept
        : target(target), pname(pname) {
      this->ptr = new GLfloat(param);
      type = glTexParameterType::kFloat;
    }
    glTexParameterValues(GLenum target, GLenum pname, GLint param) noexcept
        : target(target), pname(pname) {
      this->ptr = new GLint(param);
      type = glTexParameterType::kInteger;
    }
    glTexParameterValues(GLenum target, GLenum pname, glm::vec3 param) noexcept
        : target(target), pname(pname) {
      this->ptr = new glm::vec3(param);
      type = glTexParameterType::kFloatVec;
    }
    // modify = false -> glTexParameterIiv
    // modify = true -> glTexParameteriv
    glTexParameterValues(GLenum target, GLenum pname, glm::ivec3 param,
                         bool modify = true) noexcept
        : target(target), pname(pname) {
      this->ptr = new glm::ivec3(param);
      if (not modify) {
        type = glTexParameterType::kIIntVec;
      } else {
        type = glTexParameterType::kIntVec;
      }
    }
    glTexParameterValues(GLenum target, GLenum pname, glm::uvec3 param) noexcept
        : target(target), pname(pname) {
      this->ptr = new glm::uvec3(param);
      type = glTexParameterType::kIUIntVec;
    }
    void execute() noexcept {
      if (ptr != nullptr) {
        switch (type) {
          case glTexParameterType::kFloat:
            glTexParameterf(target, pname, (*((GLfloat*)ptr)));
            break;
          case glTexParameterType::kInteger:
            glTexParameteri(target, pname, (*((GLint*)ptr)));
            break;
          case glTexParameterType::kFloatVec:
            glTexParameterfv(target, pname, ((GLfloat*)ptr));
            break;
          case glTexParameterType::kIIntVec:
            glTexParameterIiv(target, pname, ((GLint*)ptr));
            break;
          case glTexParameterType::kIntVec:
            glTexParameteriv(target, pname, ((GLint*)ptr));
            break;
          case glTexParameterType::kIUIntVec:
            glTexParameterIuiv(target, pname, ((GLuint*)ptr));
            break;
        }
      }
    }
  };

  Texture(unsigned char* data, int width, int height, int channels,
          std::vector<glTexParameterValues> values, Type type,
          std::string path = "")
      : values_(values), type(type) {
    this->path_ = path;
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    if (data) {
      GLenum format = GL_RED;
      if (channels == 1)
        format = GL_RED;
      else if (channels == 3)
        format = GL_RGB;
      else if (channels == 4)
        format = GL_RGBA;

      glBindTexture(GL_TEXTURE_2D, id_);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      for (auto itr = values.begin(); itr != values.end(); itr++) {
        itr->execute();
      }

    } else {
      throw std::exception("Failed to load texture");
    }
  }

  ~Texture() { glDeleteTextures(1, &id_); }

  uint32_t GetID() const noexcept { return id_; }
  std::string GetPath() const noexcept { return path_; }

 protected:
  // copy constructor
  Texture(const Texture&) noexcept : type(Type::kDiffuse) {}

  // move constructor
  Texture(Texture&&) noexcept : type(Type::kDiffuse) {}

  // copy assignment
  Texture& operator=(const Texture&) noexcept { return *this; }

  // move assignment
  Texture& operator=(Texture&&) noexcept { return *this; }

  const std::string types_[17]{
      "diffuse",          "specular",       "ambient",    "emissive",
      "height",           "normals",        "shininess",  "opacity",
      "displacement",     "lightmap",       "reflection", "base_color",
      "normal_camera",    "emission_color", "metalness",  "diffuse_roughness",
      "ambient_occlusion"};

  // texture id in glfw context
  uint32_t id_ = 0;
  // Path to texture file
  std::string path_;

 private:
  const std::vector<glTexParameterValues> values_;
};

#endif