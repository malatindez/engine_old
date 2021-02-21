#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Ticker.h"
#include "engine/client/render/Renderer.h"

namespace engine::client::render {
class Renderer;
}
namespace engine::core {
class Object : public Ticker {
 public:
  Object(const uint32_t tickrate, glm::vec3 coords = glm::vec3(0.0F),
         glm::vec3 angle = glm::vec3(2 * 3.1415927F),
         glm::vec3 scale = glm::vec3(1.0F))
      : Ticker(tickrate) {
    this->Move(coords);
    this->Rotate(angle);
    this->Scale(scale);
  }
  Object(const uint32_t tickrate, std::thread::id const& thread_id,
         glm::vec3 coords = glm::vec3(0.0F),
         glm::vec3 angle = glm::vec3(2 * 3.1415927F),
         glm::vec3 scale = glm::vec3(1.0F))
      : Ticker(tickrate, thread_id) {
    this->Move(coords);
    this->Rotate(angle);
    this->Scale(scale);
  }


  [[nodiscard]] virtual std::shared_ptr<engine::client::render::Renderer>
  renderer()  {
    return {};
  }

  // This function returns rotation & coordinate matrix, which we can process &
  // use in shader
  [[nodiscard]] glm::mat4 translation_matrix() const noexcept;
  [[nodiscard]] glm::mat4 rotation_matrix() const noexcept;
  [[nodiscard]] glm::mat4 scale_matrix() const noexcept;
  [[nodiscard]] glm::mat4 model_matrix() noexcept;

  // move object by this coords(object.x += coords.x, object.y += coords.y etc.)
  void Move(glm::vec3 const& coords) noexcept;
  // move object by this coords(object.x += coords.x, object.y += coords.y etc.)
  void Move(const float x, const float y, const float z) noexcept;
  // set translation matrix
  void SetTranslationMatrix(glm::mat4 const& mat) noexcept;
  // set current position
  void SetPosition(glm::vec3 const& pos) noexcept;

  // angle should be defined in radians
  // rotate object by given angle
  void Rotate(const float anglex, const float angley,
              const float anglez) noexcept;
  // angle should be defined in radians
  // rotate object by given angle
  void Rotate(const glm::vec3 angle) noexcept;
  // angle should be defined in radians
  // rotate object by given angle
  void RotateX(const float angle) noexcept;
  // angle should be defined in radians
  // rotate object by given angle
  void RotateY(const float angle) noexcept;
  // angle should be defined in radians
  // rotate object by given angle
  void RotateZ(const float angle) noexcept;
  // set rotation matrix
  void SetRotationMatrix(glm::mat4 const& mat) noexcept;
  // set rotation angles
  void SetRotation(glm::vec3 const& angle) noexcept;
  // set rotation angles
  void SetRotation(const float anglex, const float angley,
                   const float anglez) noexcept;

  // scale object by value (transforms current scale)
  void Scale(glm::vec3 const& scale) noexcept;
  // scale object by value (transforms current scale)
  void Scale(const int x, const int y, const int z) noexcept;
  // scale object by value (transforms current scale)
  void ScaleX(const int scale) noexcept;
  // scale object by value (transforms current scale)
  void ScaleY(const int scale) noexcept;
  // scale object by value (transforms current scale)
  void ScaleZ(const int scale) noexcept;
  // set scale matrix
  void SetScaleMatrix(glm::mat4 const& mat) noexcept;
  // set current scale
  void SetScale(glm::vec3 const& scale) noexcept;

 private:
  void UpdateModelMatrix() noexcept;

  glm::mat4 translation_matrix_ = glm::mat4(1.0F);
  glm::mat4 rotation_matrix_ = glm::mat4(1.0F);
  glm::mat4 scale_matrix_ = glm::mat4(1.0F);
  glm::mat4 full_matrix_ = glm::mat4(1.0F);

  glm::mat4 translation_matrix_buf_ = glm::mat4(1.0F);
  glm::mat4 rotation_matrix_buf_ = glm::mat4(1.0F);
  glm::mat4 scale_matrix_buf_ = glm::mat4(1.0F);
};
}  // namespace engine::core