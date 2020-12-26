#pragma once

#include "../render/misc/Camera.h"
#include "FrameTicker.h"
#include "misc/Input.h"

namespace engine::core {
class Player : public FrameTicker {
 public:
  Player(Input* const input, glm::vec3 worldUp,
         glm::vec3 position = glm::vec3(0.0f))
      : input_(input), FrameTicker(1) {
    this->position_ = position;
    this->camera_ =
        std::shared_ptr<Camera>(new Camera(input, worldUp, 0, 45.0f));
  }
  void setPosition(glm::vec3 x) noexcept { this->position_ = x; }
  glm::vec3 getPosition() const noexcept { return position_; }
  void setMovementSpeed(float x) noexcept { this->speed_ = x; }
  float getMovementSpeed() const noexcept { return speed_; }
  virtual void Update(const unsigned int tick, const float delta) noexcept {
    float v = speed_ * delta;
    v = v * log10(1 + position_.y);
    if (input_->CheckSequence('W')) {
      position_ +=
          glm::vec3(
              camera_->GetFront().x / cos(glm::radians(camera_->GetPitch())), 0,
              camera_->GetFront().z / cos(glm::radians(camera_->GetPitch()))) *
          v;
    }
    if (input_->CheckSequence('A')) {
      position_ -= camera_->GetRight() * v;
    }
    if (input_->CheckSequence('S')) {
      position_ -=
          glm::vec3(
              camera_->GetFront().x / cos(glm::radians(camera_->GetPitch())), 0,
              camera_->GetFront().z / cos(glm::radians(camera_->GetPitch()))) *
          v;
    }
    if (input_->CheckSequence('D')) {
      position_ += camera_->GetRight() * v;
    }
    if (input_->CheckSequence(GLFW_KEY_LEFT_SHIFT)) {
      position_ -= camera_->GetWorldUp() * v;
    }
    if (input_->CheckSequence(GLFW_KEY_SPACE)) {
      position_ += camera_->GetWorldUp() * v;
    }
    if (position_.y < 0) {
      position_.y = 0;
    }
    this->camera_->SetPosition(this->position_);
    this->camera_->Update(tick, delta);
  }

  std::shared_ptr<Camera> camera_;
  Input* const input_;

 protected:
  glm::vec3 position_;
  float speed_ = 1.0f;
};
}  // namespace engine::core