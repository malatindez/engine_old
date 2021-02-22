#pragma once
#include <GLFW/glfw3.h>

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <iostream>
#include <algorithm>

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

  [[nodiscard]] static double time() {
    std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();
    double microseconds =
        (double)std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch())
            .count();
    return microseconds / 1e6;
  }

  [[nodiscard]] static uint64_t global_tick() noexcept {
    return core_ptr_->global_tick_;
  }

  // returns shared pointer to the Core.
  [[nodiscard]] static std::shared_ptr<Core> GetInstance() noexcept;

  /// <summary>
  /// Calculates tickrate based on how many times you want your function being
  /// called.
  /// </summary>
  /// <param name="times_per_second">how many times you want your function being
  /// called</param> <returns>Tickrate</returns>
  [[nodiscard]] static uint32_t CalcTickrate(uint32_t times_per_second) {
    return (uint32_t)ceil(double(core_ptr_->tickrate_) / times_per_second);
  }

  [[nodiscard]] static double tick_delta() {
    return core_ptr_->last_tick_timedelta_;
  }

  // returns 1 if succeed
  // 0 if failed
  int AddTickingObject(std::weak_ptr<Ticker> object) {
    auto temp = object.lock();
    if (temp == nullptr) {
      return 0;
    }
    if (temp->thread_id().expired()) {  // we can add the object to any thread
      std::scoped_lock<std::mutex> lock(threads_mutex_);

      if (threads_.empty()) {
        return 0;
      }

      auto min_execution_thread = threads_.begin();
      for (auto i = threads_.begin() + 1; i != threads_.end(); i++) {
        if ((*i).get()->exec_time() <
            min_execution_thread->get()->exec_time()) {
          min_execution_thread = i;
        }
      }
      min_execution_thread->get()->AddObject(object);
    } else {
      threads_mutex_.lock();
      auto k = temp->thread_id().lock();
      // adding object to thread with demanded id
      for (auto i = threads_.begin(); i != threads_.end(); i++) {
        if (*k == i->get()->thread_id()) {
          i->get()->AddObject(object);
          break;
        }
      }
      threads_mutex_.unlock();
    }

    return 1;
  }

  std::shared_ptr<engine::client::render::Shader> LoadShader(
      std::string const& vertex_path, std::string const& fragment_path,
      std::string const& geometry_path = "") {
    std::string key = vertex_path + "#" + fragment_path + "#" + geometry_path;
    if (shaders_.find(key) != shaders_.end()) {
      return shaders_[key];
    }
    using engine::client::render::Shader;
    std::scoped_lock<std::mutex> lock(shaders_mutex_);

    auto ptr = std::make_shared<engine::client::render::Shader>(
        Shader::LoadSourceCode(vertex_path),
        Shader::LoadSourceCode(fragment_path),
        Shader::LoadSourceCode(geometry_path));
    shaders_[key] = ptr;
    return ptr;
  }

 private:
  class UpdateThread {
   public:
    explicit UpdateThread(uint32_t tick) : local_tick_(tick) {
      this->thread_ =
          std::make_unique<std::thread>(&UpdateThread::ThreadFunction, this);
    }

    [[nodiscard]] double exec_time() const noexcept { return exec_time_; }

    void AddObject(std::weak_ptr<Ticker> object) {
      std::scoped_lock<std::mutex> lock(objects_to_add_mutex_);
      objects_to_add_.push_back(object);
    }

    [[nodiscard]] std::thread::id thread_id() const noexcept {
      return thread_->get_id();
    }
    
    
   private:
    [[noreturn]] void ThreadFunction() {
      std::shared_ptr<Core> core = Core::GetInstance();
      core->ThreadReady(thread_->get_id());
      while (true) {
        for (const auto& object : objects_) {
          object.lock()->UpdateExecutionTime(core->global_tick_);
        }
        if ((local_tick_ * core->tickrate_ / 8) % core->tickrate_ == 0 &&
            !objects_to_add_.empty()) {
          std::scoped_lock<std::mutex> lock(objects_to_add_mutex_);
          objects_.insert(objects_.end(), objects_to_add_.begin(),
                          objects_to_add_.end());
          objects_to_add_.clear();
        }
        if ((local_tick_ * core->tickrate_ / 16) % core->tickrate_ == 0) {
          for (auto itr = objects_.begin(); itr != objects_.end(); itr++) {
            exec_time_ +=
                itr->lock()->average_update_time() / itr->lock()->tickrate();
          }
        }
        core->ThreadReady(thread_->get_id());
        local_tick_ = core->global_tick_;
      }
    }

    std::mutex objects_to_add_mutex_;
    std::vector<std::weak_ptr<Ticker>> objects_to_add_;

    std::vector<std::weak_ptr<Ticker>> objects_;

    std::unique_ptr<std::thread> thread_;

    double exec_time_ = 0;

    // stores current local tick
    uint64_t local_tick_ = 0;
  };

  static std::chrono::nanoseconds calc_overhead() {
    using namespace std::chrono;
    constexpr size_t tests = 1001;
    constexpr auto timer = 200us;

    std::condition_variable t;
    auto init = [&timer, &t]() {
      auto end = steady_clock::now() + timer;
      while (steady_clock::now() < end);
      time();
      t.notify_all();
    };

    time_point<steady_clock> start;
    nanoseconds dur[tests];

    for (auto& d : dur) {
      start = steady_clock::now();
      init();
      d = steady_clock::now() - start - timer;
    }
    std::sort(std::begin(dur), std::end(dur));
    return dur[tests / 2];
  }

  const std::chrono::nanoseconds overhead_ = calc_overhead();

  inline void busy_sleep(std::chrono::nanoseconds t) const noexcept {
    auto end = std::chrono::steady_clock::now() + t - overhead_;
    while (std::chrono::steady_clock::now() < end);
  }

  void ThreadReady(std::thread::id id) {
    std::unique_lock lock(sync_mutex_);

    if (operational_thread_id_ != id) {
      sync_threads_ += 1;
      sync_var_.wait(lock);
    } 
    else {
      
      lock.unlock();
      while (sync_threads_ + 1 < threads_.size()) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
      }
      lock.lock();
      double t = time();
      double wait_t =
          (1e9 / (double)tickrate_) - 1e9 * (t - last_tick_timestamp_);
      if (wait_t > 0) {
        busy_sleep(std::chrono::nanoseconds((uint32_t)wait_t));
      }

      t = time();
      global_tick_ += 1;
      last_tick_timedelta_ = t - last_tick_timestamp_;
      last_tick_timestamp_ = t;

      sync_threads_ = 0;
      sync_var_.notify_all();
      std::cout << 1 / last_tick_timedelta_ << std::endl;
    }
  }
  std::mutex sync_mutex_;
  std::condition_variable sync_var_;
  size_t sync_threads_ = 0;

  Core() {
    last_tick_timestamp_ = time();
    for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
      auto ptr = std::make_unique<UpdateThread>(global_tick_);
      threads_.push_back(std::move(ptr));
    }
    operational_thread_id_ = threads_[0]->thread_id();

  }
  

  static std::mutex core_creation_mutex_;
  static std::shared_ptr<Core> core_ptr_;


  std::thread::id operational_thread_id_;

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