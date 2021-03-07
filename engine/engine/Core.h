#pragma once
#include <GLFW/glfw3.h>

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <iostream>
#include <algorithm>
#include <condition_variable>

#include "Ticker.h"
#include "engine/client/render/Shader.h"

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
  ~Core() {
    for (auto& thread : threads_) {
      thread.reset();
    }
  }

  [[nodiscard]] static double time();

  [[nodiscard]] static uint64_t global_tick();

  // returns shared pointer to the Core.
  [[nodiscard]] static std::shared_ptr<Core> GetInstance() noexcept;

  /// <summary>
  /// Calculates tickrate based on how many times you want your function being
  /// called.
  /// </summary>
  /// <param name="times_per_second">how many times you want your function being
  /// called</param> <returns>Tickrate</returns>
  [[nodiscard]] static uint32_t CalcTickrate(uint32_t times_per_second);

  [[nodiscard]] static double tick_delta();

  // returns 1 if succeed
  // 0 if failed
  int AddTickingObject(std::weak_ptr<Ticker> object);

  std::shared_ptr<engine::client::render::Shader> LoadShader(
      std::string const& vertex_path, std::string const& fragment_path,
      std::string const& geometry_path = "");

 private:
  class UpdateThread {
   public:
    explicit UpdateThread(uint32_t tick);
    ~UpdateThread();
    [[nodiscard]] double exec_time() const noexcept;

    void AddObject(std::weak_ptr<Ticker> object);

    [[nodiscard]] std::thread::id thread_id() const noexcept;
    
    
   private:
    [[noreturn]] void ThreadFunction();
    bool die_ = false;

    std::mutex objects_to_add_mutex_;
    std::vector<std::weak_ptr<Ticker>> objects_to_add_;

    std::vector<std::weak_ptr<Ticker>> objects_;

    std::unique_ptr<std::thread> thread_;

    double exec_time_ = 0;

    // stores current local tick
    uint64_t local_tick_ = 0;
  };

  static std::chrono::nanoseconds calc_overhead();

  const std::chrono::nanoseconds overhead_ = calc_overhead();

  inline void busy_sleep(std::chrono::nanoseconds t) const;

  void ThreadReady(std::thread::id id);

  Core();


  static std::mutex core_creation_mutex_;
  static std::shared_ptr<Core> core_ptr_;

  std::thread::id operational_thread_id_;
  std::mutex sync_mutex_;
  std::condition_variable sync_var_;
  size_t sync_threads_ = 0;


  double last_tick_timestamp_ = 0;
  double last_tick_timedelta_ = 0;

  uint64_t global_tick_ = 0;

  uint64_t tickrate_ = 64;

  std::vector<std::unique_ptr<UpdateThread>> threads_;
  std::mutex threads_mutex_;

  std::map<std::string, std::shared_ptr<engine::client::render::Shader>>
      shaders_;
  std::mutex shaders_mutex_;
};
}  // namespace engine::core