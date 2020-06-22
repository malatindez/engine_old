#include "Object.h"

void Object::UpdateFullMatrix() noexcept {
	bool flag = false;
	if (bufTranslationMatrix != translationMatrix) {
		bufTranslationMatrix = translationMatrix;
	}
	if (bufRotationMatrix != bufRotationMatrix) {
		bufRotationMatrix = rotationMatrix;
	}
	if (bufScaleMatrix != bufScaleMatrix) {
		bufScaleMatrix = scaleMatrix;
	}
	if (flag) {
		fullMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	}
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



void Object::rotate(const float anglex, const float angley, const float anglez) noexcept {
	rotationMatrix = glm::rotate(rotationMatrix, 3.1415927f, glm::vec3(anglex / 3.1415927f, angley / 3.1415927f, anglez / 3.1415927f));
}

void Object::rotate(const glm::vec3 angle) noexcept {
	this->rotate(angle.x, angle.y, angle.z);
}

void Object::rotateX(const float angle) noexcept {
	rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(1, 0, 0));
}

void Object::rotateY(const float angle) noexcept {
	rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(0, 1, 0));
}

void Object::rotateZ(const float angle) noexcept {
	rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(0, 1, 1));
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