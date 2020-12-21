#ifndef CAMERA_H
#define CAMERA_H
#define YAW -90.0f
#define PITCH 0.0f
#define SENSIVITY 0.1f
#define MAX_FOV 120.0f

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../core/input/Input.h"
class Camera : FrameTicker {
  glm::vec3 front = glm::vec3(0.0f);
  glm::vec3 up = glm::vec3(0.0f);
  glm::vec3 right = glm::vec3(0.0f);
  glm::vec3 worldUp = glm::vec3(0.0f);
  glm::vec3 position = glm::vec3(0.0f);
  float yaw = -90.0f;
  float pitch = 0.0f;
  float sensivity = 0.1f;
  float fov = 90.0f;

  const Input* const input;
  bool constrainPitch = true;

 public:
  Camera(const Input* const ref, glm::vec3 worldUp, float yaw, float pitch)
      : input(ref), FrameTicker(1) {
    this->worldUp = worldUp;
    this->yaw = yaw;
    this->pitch = pitch;
  }
  void setConstrainPitch(bool cp = true) noexcept { constrainPitch = cp; }
  bool getConstrainPitch() const noexcept { return constrainPitch; }
  float getYaw() const noexcept { return this->yaw; }
  void setYaw(const float value) noexcept { this->yaw = value; }
  float getPitch() const noexcept { return this->pitch; }
  void setPitch(const float value) noexcept { this->pitch = value; }
  float getSensivity() const noexcept { return this->sensivity; }
  void setSensivity(const float value) noexcept { this->sensivity = value; }
  float getFOV() const noexcept { return this->fov; }
  void setWorldUp(const glm::vec3 value) noexcept { this->worldUp = value; }
  glm::vec3 getWorldUp() const noexcept { return this->worldUp; }
  void setFOV(const float value) noexcept { this->fov = value; }
  glm::vec3 getFront() const noexcept { return front; }
  glm::vec3 getUp() const noexcept { return up; }
  glm::vec3 getRight() const noexcept { return right; }
  void setPosition(glm::vec3 pos) noexcept { this->position = pos; }
  glm::vec3 getPosition() const noexcept { return this->position; }

 private:
  float prevPosX = 0.0f;
  float prevPosY = 0.0f;

 public:
  virtual void Update(const unsigned int, const float) noexcept {
    auto pos = this->input->getMousePosition();

    yaw += (pos.first - prevPosX) * sensivity * fov / MAX_FOV;
    pitch += (prevPosY - pos.second) * sensivity * fov / MAX_FOV;
    prevPosX = pos.first;
    prevPosY = pos.second;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
      if (pitch > 89.9f) pitch = 89.9f;
      if (pitch < -89.9f) pitch = -89.9f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(
        this->front, worldUp));  // Normalize the vectors, because their length
                                 // gets closer to 0 the more you look up or
                                 // down which results in slower movement.
    up = glm::normalize(glm::cross(right, this->front));
  }

  glm::mat4 GetViewMatrix() const noexcept {
    return glm::lookAt(position, position + front, up);
  }
};

#undef ZOOM
#undef SENSIVITY
#undef PITCH
#undef YAW
#endif