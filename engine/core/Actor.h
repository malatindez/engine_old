#pragma once 

#include "Object.h"
#include "Ticker.h"
namespace engine::core {
class Actor : public Object, public Ticker {
 public:
  Actor(unsigned int tickrate, std::string path, TextureManager* const mngr,
        glm::vec3 coords = glm::vec3(0.0f),
        glm::vec3 angle = glm::vec3(2 * 3.1415927f),
        glm::vec3 scale = glm::vec3(1.0f))
      : Object(path, mngr, coords, angle, scale), Ticker(tickrate) {}
  Actor(unsigned int tickrate, std::vector<std::shared_ptr<Mesh>> meshes,
        TextureManager* const mngr, glm::vec3 coords = glm::vec3(0.0f),
        glm::vec3 angle = glm::vec3(3.1415927f),
        glm::vec3 scale = glm::vec3(1.0f))
      : Object(meshes, mngr, coords, angle, scale), Ticker(tickrate) {}

  virtual void Update(const unsigned int tick,
                      const float delta_time) noexcept {
    if (tick % getTickRate() == 0) {
      // this->move(glm::vec3(0, 1, 0));
    }
  }
};
}  // namespace engine::core