#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/client/misc/Window.h>
namespace engine::client::render {

class Camera {
 public:
  static const float kDefaultYaw;
  static const float kDefaultPitch;
  static const float kDefaultSensivity;
  static const float kDefaultFOV;
  static const float kMaxFOV;

  Camera(std::shared_ptr<Window> window, glm::vec3 world_up,
         float yaw = kDefaultYaw, float pitch = kDefaultPitch);

  bool constrain_pitch() const noexcept { return constrain_pitch_; }
  void SetConstrainPitch(bool cp = true) noexcept { constrain_pitch_ = cp; }
  float yaw() const noexcept { return this->yaw_; }
  void SetYaw(const float value) noexcept { this->yaw_ = value; }
  float pitch() const noexcept { return this->pitch_; }
  void SetPitch(const float value) noexcept { this->pitch_ = value; }
  float sensivity() const noexcept { return this->sensivity_; }
  void SetSensivity(const float value) noexcept { this->sensivity_ = value; }
  float FOV() const noexcept { return this->fov_; }
  void SetFOV(const float value) noexcept { this->fov_ = value; }
  glm::vec3 world_up() const noexcept { return this->world_up_; }
  void SetWorldUp(const glm::vec3 value) noexcept { this->world_up_ = value; }
  glm::vec3 position() const noexcept { return this->position_; }
  void SetPosition(glm::vec3 pos) noexcept { this->position_ = pos; }

  glm::vec3 front() const noexcept { return front_; }
  glm::vec3 up() const noexcept { return up_; }
  glm::vec3 right() const noexcept { return right_; }
  glm::mat4 view_matrix() const noexcept {
    return glm::lookAt(position_, position_ + front_, up_);
  }

  bool CursorCallback(const float posx, const float posy);

 private:
  glm::vec3 front_ = glm::vec3(0.0F);
  glm::vec3 up_ = glm::vec3(0.0F);
  glm::vec3 right_ = glm::vec3(0.0F);
  glm::vec3 world_up_ = glm::vec3(0.0F);
  glm::vec3 position_ = glm::vec3(0.0F);
  float yaw_ = kDefaultYaw;
  float pitch_ = kDefaultPitch;
  float sensivity_ = kDefaultSensivity;
  float fov_ = kDefaultFOV;

  bool constrain_pitch_ = true;

  float prev_pos_x_ = 0.0F;
  float prev_pos_y_ = 0.0F;

  std::shared_ptr<Window::CursorPosCallbackFun> cp_callback_pointer_;
};
}  // namespace engine::client::render