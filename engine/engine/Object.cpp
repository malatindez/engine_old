
#include "Object.h"

namespace engine::core {
[[nodiscard]] glm::mat4 Object::translation_matrix() const noexcept {
  return translation_matrix_;
}
[[nodiscard]] glm::mat4 Object::rotation_matrix() const noexcept { 
  return rotation_matrix_; 
}
[[nodiscard]] glm::mat4 Object::scale_matrix() const noexcept { 
  return scale_matrix_; 
}

[[nodiscard]] glm::mat4 Object::model_matrix() noexcept {
  UpdateModelMatrix();
  return full_matrix_;
}

[[nodiscard]] glm::vec3 Object::position() const noexcept {
  return glm::vec3(translation_matrix_[3][0], translation_matrix_[3][1],
                   translation_matrix_[3][2]);
}
[[nodiscard]] glm::vec3 Object::scale() const noexcept {
  return glm::vec3(scale_matrix_[0][0], scale_matrix_[1][1],
                   scale_matrix_[2][2]);
}

void Object::Move(glm::vec3 const& coords) noexcept {
  translation_matrix_ = glm::translate(translation_matrix_, coords);
}

void Object::Move(const float x, const float y, const float z) noexcept {
  translation_matrix_ = glm::translate(translation_matrix_, glm::vec3(x, y, z));
}

void Object::SetTranslationMatrix(glm::mat4 const& mat) noexcept {
  this->translation_matrix_ = mat;
}
void Object::SetPosition(glm::vec3 const& pos) noexcept {
  this->translation_matrix_ = glm::translate(glm::mat4(1.0F), pos);
}

void Object::Rotate(const float anglex, const float angley,
                    const float anglez) noexcept {
  RotateX(anglex);
  RotateY(angley);
  RotateZ(anglez);
}

void Object::Rotate(const glm::vec3 angle) noexcept {
  this->Rotate(angle.x, angle.y, angle.z);
}

void Object::RotateX(const float angle) noexcept {
  rotation_matrix_ =
      glm::rotate(rotation_matrix_, angle, glm::vec3(1.0F, 0.0F, 0.0F));
}

void Object::RotateY(const float angle) noexcept {
  rotation_matrix_ =
      glm::rotate(rotation_matrix_, angle, glm::vec3(0.0F, 1.0F, 0.0F));
}

void Object::RotateZ(const float angle) noexcept {
  rotation_matrix_ =
      glm::rotate(rotation_matrix_, angle, glm::vec3(0.0F, 0.0F, 1.0F));
}

void Object::SetRotationMatrix(glm::mat4 const& mat) noexcept {
  this->rotation_matrix_ = mat;
}
void Object::SetRotation(glm::vec3 const& angle) noexcept {
  this->rotation_matrix_ = glm::mat4(1.0F);
  this->RotateX(angle.x);
  this->RotateY(angle.y);
  this->RotateZ(angle.z);
}
void Object::SetRotation(const float anglex, const float angley,
                         const float anglez) noexcept {
  this->rotation_matrix_ = glm::mat4(1.0F);
  this->RotateX(anglex);
  this->RotateY(angley);
  this->RotateZ(anglez);
}

void Object::Scale(glm::vec3 const& scale) noexcept {
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

void Object::SetScaleMatrix(glm::mat4 const& mat) noexcept {
  this->scale_matrix_ = mat;
}

void Object::SetScale(glm::vec3 const& scale) noexcept {
  this->scale_matrix_ = glm::scale(glm::mat4(1.0F), scale);
}


void Object::UpdateModelMatrix() noexcept {
  bool flag = false;
  if (translation_matrix_buf_ != translation_matrix_) {
    translation_matrix_buf_ = translation_matrix_;
    flag = true;
  }
  if (rotation_matrix_buf_ != rotation_matrix_) {
    rotation_matrix_buf_ = rotation_matrix_;
    flag = true;
  }
  if (scale_matrix_buf_ != scale_matrix_) {
    scale_matrix_buf_ = scale_matrix_;
    flag = true;
  }
  if (flag) {
    full_matrix_ = translation_matrix_ * rotation_matrix_ * scale_matrix_;
  }
}
}  // namespace engine::core