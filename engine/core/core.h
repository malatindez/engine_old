#ifndef CORE_H
#define CORE_H
#include <memory>
#include <vector>

#include "../render/render.h"
#include "Ticker.h"
namespace engine::core {
class Core {
 public:
  unsigned int current_tick = 0;
  std::vector<std::shared_ptr<Ticker>> tickingObjects;
  Core() {}

  unsigned int getCurrentGlobalTick() const noexcept { return current_tick; }

  bool appendTickingObject(std::shared_ptr<Ticker> object) {

  }
};
}  // namespace engine::core
#endif