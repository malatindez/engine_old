#pragma once

#include <engine/client/render/Camera.h>

#include "misc/Input.h"

namespace engine::client {
class Player {
 public:
  /* Disable copy and move semantics. */
  Player(const Player&) = delete;
  Player(Player&&) = delete;
  Player& operator=(const Player&) = delete;
  Player& operator=(Player&&) = delete;

  Player(std::shared_ptr<Window> window, glm::vec3 world_up,
         glm::vec3 position = glm::vec3(0.0F)) {
    this->camera_ =
        std::make_shared<render::Camera>(window, world_up, 0, 45.0F);
    camera_->SetPosition(position);
    using namespace std::placeholders;

    this->move_cb_ptr_ = std::make_shared<Window::KeyBindCallback>(
        std::bind(&Player::Move, this, _1, _2));

    window->AddKeyCallback(glfwGetKeyScancode(GLFW_KEY_W), move_cb_ptr_);
    window->AddKeyCallback(glfwGetKeyScancode(GLFW_KEY_A), move_cb_ptr_);
    window->AddKeyCallback(glfwGetKeyScancode(GLFW_KEY_S), move_cb_ptr_);
    window->AddKeyCallback(glfwGetKeyScancode(GLFW_KEY_D), move_cb_ptr_);
  }

  void SetPosition(glm::vec3 x) noexcept { camera_->SetPosition(x); }
  [[nodiscard]] glm::vec3 position() const noexcept { return camera_->position(); }

  void SetMovementSpeed(float x) noexcept { this->speed_ = x; }
  [[nodiscard]] float movement_speed() const noexcept { return speed_; }

  [[nodiscard]] std::shared_ptr<render::Camera> camera() const { return camera_; }

 private:
   
  bool Move(int32_t scancode, int32_t action) {
    if (action == 0) {
      return false;
    }
    glm::vec3 position = camera_->position();
    if (scancode == kScancodeW) {
      position += speed_ * camera_->front();
    } else if (scancode == kScancodeS) {
      position -= speed_ * camera_->front();
    } else if (scancode == kScancodeA) {
      position -=
          glm::normalize(glm::cross(camera_->front(), camera_->up())) * speed_;
    } else if (scancode == kScancodeD) {
      position +=
          glm::normalize(glm::cross(camera_->front(), camera_->up())) * speed_;
    }
    camera_->SetPosition(position);
    return false; 
  }

  const int32_t kScancodeW = glfwGetKeyScancode(GLFW_KEY_W);
  const int32_t kScancodeA = glfwGetKeyScancode(GLFW_KEY_A);
  const int32_t kScancodeS = glfwGetKeyScancode(GLFW_KEY_S);
  const int32_t kScancodeD = glfwGetKeyScancode(GLFW_KEY_D);

  std::shared_ptr<render::Camera> camera_;

  float speed_ = 0.05F;

  std::shared_ptr<Window::KeyBindCallback> move_cb_ptr_;
};
}  // namespace engine::client::client