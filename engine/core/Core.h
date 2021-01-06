#pragma once
#include <memory>
#include <vector>

#include "../render/render.h"
#include "Ticker.h"

namespace engine::core {

// Singleton class instance of which you can get by calling the GetInstance
// function.
class Core final {
 public:

  /* Disable copy and move semantics. */
  Core(const Core&) = delete;
  Core(Core&&) = delete;
  Core& operator=(const Core&) = delete;
  Core& operator=(Core&&) = delete;

  [[nodiscard]] uint64_t GetCurrentGlobalTick() const noexcept {
    return current_tick_;
  }

  // returns reference to the Core.
  [[nodiscard]] static std::shared_ptr<Core> GetInstance() noexcept;

 private:
  Core() = default;
  static std::shared_ptr<Core> core_ptr_;
  
  uint64_t current_tick_ = 0;

  std::vector<std::shared_ptr<Ticker>> ticking_objects_;
};
}  // namespace engine::core