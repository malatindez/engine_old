#pragma once

#include <render/misc/Camera.h>
#include "misc/Input.h"

namespace engine::core {
class Player {
 public:
  Player(std::shared_ptr<Window> window, glm::vec3 worldUp,
         glm::vec3 position = glm::vec3(0.0F)) : position_(position) {
    this->camera_ = std::make_shared<engine::render::Camera>(
        engine::render::Camera(window, worldUp, 0, 45.0F));
  }
  void setPosition(glm::vec3 x) noexcept { this->position_ = x; }
  glm::vec3 position() const noexcept { return position_; }
  void setMovementSpeed(float x) noexcept { this->speed_ = x; }
  float movement_speed() const noexcept { return speed_; }

 private:
  std::shared_ptr<engine::render::Camera> camera_;

  glm::vec3 position_;
  float speed_ = 1.0F;
};
}  // namespace engine::core