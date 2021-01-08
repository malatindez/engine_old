#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include "Ticker.h"

namespace engine::core {

// Singleton class instance of which you can get by calling the GetInstance
// function.
// GetInstance SHOULD be called before calling any static function inside Core.
// Otherwise std::terminate will be called because core_ptr_ is equal to
// nullptr.
class Core final {
 public:
  /* Disable copy and move semantics. */
  Core(const Core&) = delete;
  Core(Core&&) = delete;
  Core& operator=(const Core&) = delete;
  Core& operator=(Core&&) = delete;

  [[nodiscard]] static uint64_t GetCurrentGlobalTick() noexcept {
    return core_ptr_->global_tick_;
  }

  // returns reference to the Core.
  [[nodiscard]] static std::shared_ptr<Core> GetInstance() noexcept;

  /// <summary>
  /// Calculates tickrate based on how many times you want your function being called.
  /// </summary>
  /// <param name="times_per_second">how many times you want your function being called</param>
  /// <returns>Tickrate</returns>
  [[nodiscard]] static uint32_t CalcTickrate(uint32_t times_per_second) {
    return (uint32_t)ceil(double(core_ptr_->tickrate_) / times_per_second);
  }

 private:
  struct ThreadData {
    std::vector<std::weak_ptr<Ticker>> ticking_objects;
    std::mutex ticking_objects_mutex;

    std::thread::id thread_id;
    
    uint64_t local_tick;
  };

  Core() = default;
  static std::shared_ptr<Core> core_ptr_;

  uint64_t global_tick_ = 0;
  uint64_t tickrate_ = 0;

  std::vector<std::weak_ptr<Ticker>> all_ticking_objects_;

};
}  // namespace engine::core