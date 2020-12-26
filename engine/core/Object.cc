#include "Object.h"

#include <iostream>
namespace engine::core {
void Object::UpdateFullMatrix() noexcept {
  bool flag = false;
  if (buf_translation_matrix_ != translation_matrix_) {
    buf_translation_matrix_ = translation_matrix_;
    flag = true;
  }
  if (buf_rotation_matrix_ != rotation_matrix) {
    buf_rotation_matrix_ = rotation_matrix;
    flag = true;
  }
  if (buf_scale_matrix_ != scale_matrix_) {
    buf_scale_matrix_ = scale_matrix_;
    flag = true;
  }
  if (flag) {
    full_matrix_ = translation_matrix_ * rotation_matrix * scale_matrix_;
  }
}
glm::mat4 Object::GetObjectMatrix() noexcept {
  UpdateFullMatrix();
  return full_matrix_;
}

void Object::Move(const glm::vec3 coords) noexcept {
  translation_matrix_ = glm::translate(translation_matrix_, coords);
}

void Object::Move(const float x, const float y, const float z) noexcept {
  translation_matrix_ = glm::translate(translation_matrix_, glm::vec3(x, y, z));
}

void Object::SetTranslationMatrix(const glm::mat4 mat) noexcept {
  this->translation_matrix_ = mat;
}
void Object::SetPosition(const glm::vec3 pos) noexcept {
  this->translation_matrix_ = glm::translate(glm::mat4(1.0f), pos);
}

void Object::Rotate(const float anglex, const float angley,
                    const float anglez) noexcept {
  RotateX(anglex);
  RotateX(angley);
  RotateX(anglez);
}

void Object::Rotate(const glm::vec3 angle) noexcept {
  this->Rotate(angle.x, angle.y, angle.z);
}

void Object::RotateX(const float angle) noexcept {
  rotation_matrix =
      glm::rotate(rotation_matrix, angle, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Object::RotateY(const float angle) noexcept {
  rotation_matrix =
      glm::rotate(rotation_matrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Object::RotateZ(const float angle) noexcept {
  rotation_matrix =
      glm::rotate(rotation_matrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Object::SetRotationMatrix(const glm::mat4 mat) noexcept {
  this->rotation_matrix = mat;
}
void Object::SetRotation(const glm::vec3 angle) noexcept {
  this->rotation_matrix = glm::mat4(1.0f);
  this->RotateX(angle.x);
  this->RotateY(angle.y);
  this->RotateZ(angle.z);
}
void Object::SetRotation(const float anglex, const float angley,
                         const float anglez) noexcept {
  this->rotation_matrix = glm::mat4(1.0f);
  this->RotateX(anglex);
  this->RotateY(angley);
  this->RotateZ(anglez);
}

void Object::Scale(const glm::vec3 scale) noexcept {
  scale_matrix_ = glm::scale(scale_matrix_, scale);
}

void Object::Scale(const int x, const int y, const int z) noexcept {
  scale_matrix_ = glm::scale(scale_matrix_, glm::vec3(x, y, z));
}
void Object::ScaleX(const int scale) noexcept {
  scale_matrix_ = glm::scale(scale_matrix_, glm::vec3(scale, 0, 0));
}
void Object::ScaleY(const int scale) noexcept {
  scale_matrix_ = glm::scale(scale_matrix_, glm::vec3(0, scale, 0));
}
void Object::ScaleZ(const int scale) noexcept {
  scale_matrix_ = glm::scale(scale_matrix_, glm::vec3(0, 0, scale));
}

void Object::SetScaleMatrix(const glm::mat4 mat) noexcept {
  this->scale_matrix_ = mat;
}

void Object::SetScale(const glm::vec3 scale) noexcept {
  this->scale_matrix_ = glm::scale(glm::mat4(1.0f), scale);
}
}  // namespace engine::core