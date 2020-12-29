#include "Camera.h"

namespace engine::render {
const float Camera::kDefaultYaw = -90.0f;
const float Camera::kDefaultPitch = 0;
const float Camera::kDefaultSensivity = 0.1f;
const float Camera::kDefaultFOV = 90.0f;
const float Camera::kMaxFOV = 120.0f;
Camera::Camera(std::shared_ptr<core::Window> window, glm::vec3 world_up,
               float yaw, float pitch) {
  this->world_up_ = world_up;
  this->yaw_ = yaw;
  this->pitch_ = pitch;
  using namespace std::placeholders;
  this->cp_callback_pointer_ =
      std::make_shared<core::Window::CursorPosCallbackFun>(
          std::bind(&Camera::CursorCallback, this, _1, _2));
  window->PushCursorPosCallback(cp_callback_pointer_);
}

bool Camera::CursorCallback(const float posx, const float posy) {
  std::cout << posx << " " << posy << std::endl;
  yaw_ += (posx - prev_pos_x_) * sensivity_ * fov_ / kMaxFOV;
  pitch_ += (prev_pos_y_ - posy) * sensivity_ * fov_ / kMaxFOV;
  prev_pos_x_ = posx;
  prev_pos_y_ = posy;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrain_pitch_) {
    if (pitch_ > 89.9f) {
      pitch_ = 89.9f;
    }
    if (pitch_ < -89.9f) {
      pitch_ = -89.9f;
    }
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
  return false;
}

}  // namespace engine::render