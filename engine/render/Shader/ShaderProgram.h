#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#ifdef CERR_OUTPUT
#include <iostream>
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
class ShaderProgram {
 private:
  unsigned int ID = 0;
  // Locking out move and copy constructors
  ShaderProgram(const ShaderProgram&) {}

  ShaderProgram(ShaderProgram&&) noexcept {}

  ShaderProgram& operator=(const ShaderProgram&) {}

  ShaderProgram& operator=(ShaderProgram&&) noexcept {}

  GLint checkCompileErrors(GLuint shader, std::string type);

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

  void use() const noexcept { glUseProgram(ID); }
  void operator()() const noexcept { use(); }
  void setBool(const std::string& name, bool value) const;
  void setInt(const std::string& name, int value) const;
  void setUInt(const std::string& name, unsigned int value) const;
  void setFloat(const std::string& name, float value) const;

  void setVec1(const std::string& name, const glm::vec1& value) const;
  void setVec2(const std::string& name, const glm::vec2& value) const;
  void setVec3(const std::string& name, const glm::vec3& value) const;
  void setVec4(const std::string& name, const glm::vec4& value) const;

  void setMat2(const std::string& name, const glm::mat2& value) const;
  void setMat3(const std::string& name, const glm::mat3& value) const;
  void setMat4(const std::string& name, const glm::mat4& value) const;

  void setMat2x2(const std::string& name, const glm::mat2x2& value) const;
  void setMat2x3(const std::string& name, const glm::mat2x3& value) const;
  void setMat2x4(const std::string& name, const glm::mat2x4& value) const;

  void setMat3x2(const std::string& name, const glm::mat3x2& value) const;
  void setMat3x3(const std::string& name, const glm::mat3x3& value) const;
  void setMat3x4(const std::string& name, const glm::mat3x4& value) const;

  void setMat4x2(const std::string& name, const glm::mat4x2& value) const;
  void setMat4x3(const std::string& name, const glm::mat4x3& value) const;
  void setMat4x4(const std::string& name, const glm::mat4x4& value) const;
};

#endif