#include "Object.h"
#include "Ticker.h"
template<int tickrate>
class Actor : public Object, public Ticker<tickrate> {
public:
	unsigned int getTickRate() const noexcept {
		return tickrate;
	}
	virtual void Update(const unsigned int tick, const float deltaTime) noexcept {
		if (tick % getTickRate() == 0) {
			this->rotateY(360.0f * deltaTime);
		}
	}
	glm::vec3 velocity = glm::vec3(0.0f), acceleration = glm::vec3(0.0f, 0.01f, 0.0f);
	void Move(const float deltaTime) noexcept {
			float m = deltaTime * deltaTime;
			this->move(this->velocity * deltaTime + acceleration * m);
			this->velocity = this->velocity + acceleration * m;
	}
	
	Actor(std::string path, TextureManager* const mngr,
		glm::vec3 coords = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(3.1415927f),
		glm::vec3 scale = glm::vec3(1.0f)) : Object(path, mngr, coords, angle, scale) {

	}
};