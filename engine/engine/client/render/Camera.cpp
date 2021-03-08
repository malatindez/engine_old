#include "Camera.h"

namespace engine::client::render {
const float Camera::kDefaultYaw = glm::radians(-90.0F);
const float Camera::kDefaultPitch = 0;
const float Camera::kDefaultSensivity = glm::radians(0.1F);
const float Camera::kDefaultFOV = glm::radians(90.0F);
const float Camera::kMaxFOV = glm::radians(120.0F);

Camera::Camera(std::shared_ptr<Window> window, glm::vec3 world_up,
               float yaw, float pitch)
    : world_up_(world_up), yaw_(yaw), pitch_(pitch) {
  using namespace std::placeholders;
  this->cp_callback_pointer_ =
      std::make_shared<Window::CursorPosCallbackFun>(
          std::bind(&Camera::CursorCallback, this, _1, _2));
  window->PushCursorPosCallback(cp_callback_pointer_);
}

bool Camera::CursorCallback(const float posx, const float posy) {
  yaw_ += (posx - prev_pos_x_) * sensivity_ * fov_ / kMaxFOV;
  pitch_ += (prev_pos_y_ - posy) * sensivity_ * fov_ / kMaxFOV;
  prev_pos_x_ = posx;
  prev_pos_y_ = posy;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrain_pitch_) {
    if (pitch_ > glm::radians(89.9F)) {
      pitch_ = glm::radians(89.9F);
    }
    if (pitch_ < glm::radians(-89.9F)) {
      pitch_ = glm::radians(-89.9F);
    }
  }

  // Update Front, Right and Up Vectors using the updated Euler angles
  glm::vec3 front;
  front.x = cos(yaw_) * cos(pitch_);
  front.y = sin(pitch_);
  front.z = sin(yaw_) * cos(pitch_);
  this->front_ = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  right_ = glm::normalize(
      glm::cross(this->front_,
                 world_up_));  // Normalize the vectors, because their length
                               // gets closer to 0 the more you look up or
                               // down which results in slower movement.
  up_ = glm::normalize(glm::cross(right_, this->front_));
  return false;
}

}  // namespace engine::client::render