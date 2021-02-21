
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace engine::client::render {
class Shader {
   public:

    /* Disable copy and move semantics. */
    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;

    Shader(std::string_view vertex_shader_code,
           std::string_view fragment_shader_code,
           std::string_view geometry_shader_code = "");

    ~Shader();

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
    static int32_t Shader::CompileShader(std::string_view shader_code, uint32_t& id);

    // shader program id
    unsigned int sp_id_ = 0;
};
}  // namespace engine::client::render