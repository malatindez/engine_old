#include "Object.h"

#include <iostream>
int i = 0;
void Object::UpdateFullMatrix() noexcept {
  bool flag = false;
  if (bufTranslationMatrix != translationMatrix) {
    bufTranslationMatrix = translationMatrix;
    flag = true;
  }
  if (bufRotationMatrix != rotationMatrix) {
    bufRotationMatrix = rotationMatrix;
    flag = true;
  }
  if (bufScaleMatrix != scaleMatrix) {
    bufScaleMatrix = scaleMatrix;
    flag = true;
  }
  if (flag) {
    fullMatrix = translationMatrix * rotationMatrix * scaleMatrix;
  }
  i++;
}
glm::mat4 Object::getObjectMatrix() noexcept {
  UpdateFullMatrix();
  return fullMatrix;
}

void Object::move(const glm::vec3 coords) noexcept {
  translationMatrix = glm::translate(translationMatrix, coords);
}

void Object::move(const float x, const float y, const float z) noexcept {
  translationMatrix = glm::translate(translationMatrix, glm::vec3(x, y, z));
}

void Object::setTranslationMatrix(const glm::mat4 mat) noexcept {
  this->translationMatrix = mat;
}
void Object::setPosition(const glm::vec3 pos) noexcept {
  this->translationMatrix = glm::translate(glm::mat4(1.0f), pos);
}

void Object::rotate(const float anglex, const float angley,
                    const float anglez) noexcept {
  rotateX(anglex);
  rotateX(angley);
  rotateX(anglez);
}

void Object::rotate(const glm::vec3 angle) noexcept {
  this->rotate(angle.x, angle.y, angle.z);
}

void Object::rotateX(const float angle) noexcept {
  rotationMatrix =
      glm::rotate(rotationMatrix, angle, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Object::rotateY(const float angle) noexcept {
  rotationMatrix =
      glm::rotate(rotationMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Object::rotateZ(const float angle) noexcept {
  rotationMatrix =
      glm::rotate(rotationMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Object::setRotationMatrix(const glm::mat4 mat) noexcept {
  this->rotationMatrix = mat;
}
void Object::setRotation(const glm::vec3 angle) noexcept {
  this->rotationMatrix = glm::mat4(1.0f);
  this->rotateX(angle.x);
  this->rotateY(angle.y);
  this->rotateZ(angle.z);
}
void Object::setRotation(const float anglex, const float angley,
                         const float anglez) noexcept {
  this->rotationMatrix = glm::mat4(1.0f);
  this->rotateX(anglex);
  this->rotateY(angley);
  this->rotateZ(anglez);
}

void Object::scale(const glm::vec3 scale) noexcept {
  scaleMatrix = glm::scale(scaleMatrix, scale);
}

void Object::scale(const int x, const int y, const int z) noexcept {
  scaleMatrix = glm::scale(scaleMatrix, glm::vec3(x, y, z));
}
void Object::scaleX(const int scale) noexcept {
  scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, 0, 0));
}
void Object::scaleY(const int scale) noexcept {
  scaleMatrix = glm::scale(scaleMatrix, glm::vec3(0, scale, 0));
}
void Object::scaleZ(const int scale) noexcept {
  scaleMatrix = glm::scale(scaleMatrix, glm::vec3(0, 0, scale));
}

void Object::setScaleMatrix(const glm::mat4 mat) noexcept {
  this->scaleMatrix = mat;
}

void Object::setScale(const glm::vec3 scale) noexcept {
  this->scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
}