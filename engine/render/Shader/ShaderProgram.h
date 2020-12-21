#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#ifdef CERR_OUTPUT
#include <iostream>
#endif
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
class ShaderProgram {
 public:
  enum class exceptions {
    FRAGMENT_COMPILATION_ERROR,
    VERTEX_COMPILATION_ERROR,
    GEOMETRY_COMPILATION_ERROR,
    PROGRAM_COMPILATION_ERROR
  };
  ShaderProgram(std::string vertexShaderCode, std::string fragmentShaderCode,
                std::string geometryShaderCode = "");

  ~ShaderProgram();

  void operator()() const noexcept { Use(); }

  void Use() const noexcept { glUseProgram(sp_id_); }

  void SetBool(const std::string& name, bool value) const;
  void SetInt(const std::string& name, int value) const;
  void SetUInt(const std::string& name, unsigned int value) const;
  void SetFloat(const std::string& name, float value) const;

  void SetVec1(const std::string& name, const glm::vec1& value) const;
  void SetVec2(const std::string& name, const glm::vec2& value) const;
  void SetVec3(const std::string& name, const glm::vec3& value) const;
  void SetVec4(const std::string& name, const glm::vec4& value) const;

  void SetMat2(const std::string& name, const glm::mat2& value) const;
  void SetMat3(const std::string& name, const glm::mat3& value) const;
  void SetMat4(const std::string& name, const glm::mat4& value) const;

  void SetMat2x2(const std::string& name, const glm::mat2x2& value) const;
  void SetMat2x3(const std::string& name, const glm::mat2x3& value) const;
  void SetMat2x4(const std::string& name, const glm::mat2x4& value) const;

  void SetMat3x2(const std::string& name, const glm::mat3x2& value) const;
  void SetMat3x3(const std::string& name, const glm::mat3x3& value) const;
  void SetMat3x4(const std::string& name, const glm::mat3x4& value) const;

  void SetMat4x2(const std::string& name, const glm::mat4x2& value) const;
  void SetMat4x3(const std::string& name, const glm::mat4x3& value) const;
  void SetMat4x4(const std::string& name, const glm::mat4x4& value) const;

 private:
  // Locking out move and copy constructors
  ShaderProgram(const ShaderProgram&) {}

  ShaderProgram(ShaderProgram&&) noexcept {}

  ShaderProgram& operator=(const ShaderProgram&) {}

  ShaderProgram& operator=(ShaderProgram&&) noexcept {}

  GLint checkCompileErrors(GLuint shader, std::string type);

  // shader program id
  unsigned int sp_id_ = 0;
};

#endif