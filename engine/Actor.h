#include "Object.h"
#include "Ticker.h"
template<int tickrate>
class Actor : public Object, public Ticker<tickrate> {
public:
	virtual void Update(const unsigned int tick) const noexcept {

	}
	Actor(std::string path, TextureManager* const mngr,
		glm::vec3 coords = glm::vec3(1.0f), glm::vec3 angle = glm::vec3(1.0f),
		glm::vec3 scale = glm::vec3(1.0f)) : Object(path, mngr, coords, angle, scale) {

	}
};