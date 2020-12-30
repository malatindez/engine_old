#pragma once
#include <memory>
#include <vector>

#include "../render/render.h"
#include "Ticker.h"

namespace engine::core {

// Singleton class instance of which you can get by calling the GetInstance
// function.
class Core {
 public:

  /* Disable copy and move semantics. */
  Core(const Core&) = delete;
  Core(Core&&) = delete;
  Core& operator=(const Core&) = delete;
  Core& operator=(Core&&) = delete;

  unsigned int GetCurrentGlobalTick() const noexcept { return current_tick; }

  // returns reference to the Core.
  static std::shared_ptr<Core> GetInstance() noexcept;

 private:
  Core() = default;
  static std::shared_ptr<Core> core_ptr_;
  
  unsigned int current_tick = 0;

  std::vector<std::shared_ptr<Ticker>> tickingObjects;
};
}  // namespace engine::core