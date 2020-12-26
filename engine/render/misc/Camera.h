#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../core/misc/Input.h"
namespace engine::render {

class Camera : FrameTicker {
 public:
  const float kDefaultYaw = -90.0f;
  const float kDefaultPitch = 0;
  const float kDefaultSensivity = 0.1f;
  const float kDefaultFOV = 90.0f;
  const float kMaxFOV = 120.0f;

  Camera(const Input* const ref, glm::vec3 world_up, float yaw, float pitch)
      : input_(ref), FrameTicker(1) {
    this->world_up_ = world_up;
    this->yaw_ = yaw;
    this->pitch_ = pitch;
  }

  bool GetConstrainPitch() const noexcept { return constrain_pitch_; }
  void SetConstrainPitch(bool cp = true) noexcept { constrain_pitch_ = cp; }
  float GetYaw() const noexcept { return this->yaw_; }
  void SetYaw(const float value) noexcept { this->yaw_ = value; }
  float GetPitch() const noexcept { return this->pitch_; }
  void SetPitch(const float value) noexcept { this->pitch_ = value; }
  float GetSensivity() const noexcept { return this->sensivity_; }
  void SetSensivity(const float value) noexcept { this->sensivity_ = value; }
  float GetFOV() const noexcept { return this->fov_; }
  void SetFOV(const float value) noexcept { this->fov_ = value; }
  glm::vec3 GetWorldUp() const noexcept { return this->world_up_; }
  void SetWorldUp(const glm::vec3 value) noexcept { this->world_up_ = value; }
  glm::vec3 GetPosition() const noexcept { return this->position_; }
  void SetPosition(glm::vec3 pos) noexcept { this->position_ = pos; }

  glm::vec3 GetFront() const noexcept { return front_; }
  glm::vec3 GetUp() const noexcept { return up_; }
  glm::vec3 GetRight() const noexcept { return right_; }
  glm::mat4 GetViewMatrix() const noexcept {
    return glm::lookAt(position_, position_ + front_, up_);
  }

  virtual void Update(const unsigned int, const float) noexcept {
    auto pos = this->input_->getMousePosition();

    yaw_ += (pos.first - prev_pos_x_) * sensivity_ * fov_ / kMaxFOV;
    pitch_ += (prev_pos_y_ - pos.second) * sensivity_ * fov_ / kMaxFOV;
    prev_pos_x_ = pos.first;
    prev_pos_y_ = pos.second;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch_) {
      if (pitch_ > 89.9f) pitch_ = 89.9f;
      if (pitch_ < -89.9f) pitch_ = -89.9f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    this->front_ = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    right_ = glm::normalize(
        glm::cross(this->front_,
                   world_up_));  // Normalize the vectors, because their length
                                 // gets closer to 0 the more you look up or
                                 // down which results in slower movement.
    up_ = glm::normalize(glm::cross(right_, this->front_));
  }

 protected:
  glm::vec3 front_ = glm::vec3(0.0f);
  glm::vec3 up_ = glm::vec3(0.0f);
  glm::vec3 right_ = glm::vec3(0.0f);
  glm::vec3 world_up_ = glm::vec3(0.0f);
  glm::vec3 position_ = glm::vec3(0.0f);
  float yaw_ = kDefaultYaw;
  float pitch_ = kDefaultPitch;
  float sensivity_ = kDefaultSensivity;
  float fov_ = kDefaultFOV;

  const Input* const input_;
  bool constrain_pitch_ = true;

 private:
  float prev_pos_x_ = 0.0f;
  float prev_pos_y_ = 0.0f;
};
}  // namespace engine::render