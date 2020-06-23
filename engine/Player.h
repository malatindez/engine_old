#include "FrameTicker.h"
#include "Camera.h"
#include "Input.h"
class Player : public FrameTicker<1> {
	glm::vec3 position;
	float speed = 9.0f;
public:
	std::shared_ptr<Camera> camera;
	const Input* const input;
	Player(Input* const input, glm::vec3 worldUp, glm::vec3 position = glm::vec3(0.0f)) : input(input) {
		this->position = position;
		input->addCheckingKey('W');
		input->addCheckingKey('A');
		input->addCheckingKey('S');
		input->addCheckingKey('D');
		input->addCheckingKey(GLFW_KEY_LEFT_SHIFT);
		input->addCheckingKey(GLFW_KEY_SPACE);
		this->camera = std::shared_ptr<Camera>(new Camera(input, worldUp, 0, 45.0f));
	}
	void setPosition(glm::vec3 x) noexcept {
		this->position = x;
	}
	glm::vec3 getPosition() const noexcept {
		return position;
	}
	void setMovementSpeed(float x) noexcept {
		this->speed = x;
	}
	float getMovementSpeed() const noexcept {
		return speed;
	}
	virtual void Update(const unsigned int tick, const float delta) noexcept {
		float v = speed * delta;
		if (input->checkSequence('W')) {
			position += glm::vec3(
				camera->getFront().x / cos(glm::radians(camera->getPitch())),
				0,
				camera->getFront().z / cos(glm::radians(camera->getPitch()))
			) * v;
		}
		if (input->checkSequence('A')) {
			position -= camera->getRight() * v;
		}
		if (input->checkSequence('S')) {
			position -= glm::vec3(
				camera->getFront().x / cos(glm::radians(camera->getPitch())),
				0,
				camera->getFront().z / cos(glm::radians(camera->getPitch()))
			) * v;
		}
		if (input->checkSequence('D')) {
			position += camera->getRight() * v;
		}
		if (input->checkSequence(GLFW_KEY_LEFT_SHIFT)) {
			position -= camera->getWorldUp() * v;
		}
		if (input->checkSequence(GLFW_KEY_SPACE)) {
			position += camera->getWorldUp() * v;
		}
		this->camera->setPosition(this->position);
		this->camera->Update(tick, delta);
	}
};