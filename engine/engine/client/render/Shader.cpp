#include "Shader.h"
namespace engine::client::render {

int32_t Shader::CompileShader(std::string_view shader_code, uint32_t& id,
                              GLenum type) {
  id = glCreateShader(type);
  const char* shader_code_c_str = shader_code.data();
  glShaderSource(id, 1, &shader_code_c_str, nullptr);
  glCompileShader(id);
  int32_t success = 0;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[1024];
    glGetShaderInfoLog(id, 1024, nullptr, info_log);
    std::cout << info_log << std::endl;
    // TODO exception output
  }
  return success;
}
std::size_t Shader::ShaderSource::hash() const noexcept {
  return std::hash<ShaderSource>()(*this);
}

Shader::Shader(ShaderSource const& source) {
  uint32_t vertex = -1;
  uint32_t fragment = -1;
  uint32_t geometry = -1;

  if (!CompileShader(source.vertex_shader_code, vertex, GL_VERTEX_SHADER)) {
    glDeleteShader(vertex);
    return;
  }

  if (!CompileShader(source.fragment_shader_code, fragment,
                     GL_FRAGMENT_SHADER)) {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return;
  }

  if (source.geometry_shader_code != "" &&
      !CompileShader(source.geometry_shader_code, geometry,
                     GL_GEOMETRY_SHADER)) {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
    return;
  }

  // shader Program
  sp_id_ = glCreateProgram();
  glAttachShader(sp_id_, vertex);
  glAttachShader(sp_id_, fragment);
  if (source.geometry_shader_code != "") {
    glAttachShader(sp_id_, geometry);
  }
  glLinkProgram(sp_id_);

  int32_t compile_success = 0;
  glGetProgramiv(sp_id_, GL_LINK_STATUS, &compile_success);

  if (!compile_success) {
    // TODO exception output
    GLchar info_log[1024];
    glGetProgramInfoLog(sp_id_, 1024, nullptr, info_log);
    std::cout << info_log << std::endl;

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (source.geometry_shader_code != "") {
      glDeleteShader(geometry);
    }
    return;
  }
  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (source.geometry_shader_code != "") {
    glDeleteShader(geometry);
  }
}

Shader::~Shader() { glDeleteProgram(sp_id_); }

void Shader::SetBool(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(sp_id_, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(sp_id_, name.c_str()), value);
}

void Shader::SetUInt(const std::string& name, unsigned int value) const {
  glUniform1ui(glGetUniformLocation(sp_id_, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(sp_id_, name.c_str()), value);
}

void Shader::SetVec1(const std::string& name, const glm::vec1& value) const {
  GLint location = glGetUniformLocation(sp_id_, name.c_str());
  glUniform1fv(location, 1, &value[0]);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const {
  GLint location = glGetUniformLocation(sp_id_, name.c_str());
  glUniform2fv(location, 1, &value[0]);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
  GLint location = glGetUniformLocation(sp_id_, name.c_str());
  glUniform3fv(location, 1, &value[0]);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const {
  GLint location = glGetUniformLocation(sp_id_, name.c_str());
  glUniform4fv(location, 1, &value[0]);
}

void Shader::SetMat2(const std::string& name, const glm::mat2& value) const {
  glUniformMatrix2fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) const {
  glUniformMatrix3fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
  glUniformMatrix4fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}

void Shader::SetMat2x2(const std::string& name,
                       const glm::mat2x2& value) const {
  SetMat2(name, value);
}

void Shader::SetMat2x3(const std::string& name,
                       const glm::mat2x3& value) const {
  glUniformMatrix2x3fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetMat2x4(const std::string& name,
                       const glm::mat2x4& value) const {
  glUniformMatrix2x4fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetMat3x2(const std::string& name,
                       const glm::mat3x2& value) const {
  glUniformMatrix3x2fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetMat3x3(const std::string& name,
                       const glm::mat3x3& value) const {
  SetMat3(name, value);
}

void Shader::SetMat3x4(const std::string& name,
                       const glm::mat3x4& value) const {
  glUniformMatrix2x4fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetMat4x2(const std::string& name,
                       const glm::mat4x2& value) const {
  glUniformMatrix4x2fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetMat4x3(const std::string& name,
                       const glm::mat4x3& value) const {
  glUniformMatrix4x2fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetMat4x4(const std::string& name,
                       const glm::mat4x4& value) const {
  glUniformMatrix4fv(glGetUniformLocation(sp_id_, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}
}  // namespace engine::client::render