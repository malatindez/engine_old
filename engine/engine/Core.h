#pragma once
#include <GLFW/glfw3.h>

#include <memory>
#include <mutex>
#include <vector>

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

  // returns 1 if succeed
  // 0 if failed
  int AddTickingObject(std::weak_ptr<Ticker> object) {
    auto temp = object.lock();
    if (temp == nullptr) {
      return 0;
    }
    if (temp->thread_id().expired()) {  // we can add the object to any thread
      std::scoped_lock<std::mutex> lock(threads_mutex);

      if (threads.empty()) {
        return 0;
      }

      auto min_execution_thread = threads.begin();
      for (auto i = threads.begin() + 1; i != threads.end(); i++) {
        if ((*i).get()->exec_time() <
            min_execution_thread->get()->exec_time()) {
          min_execution_thread = i;
        }
      }
      min_execution_thread->get()->AddObject(object);
    } else {
      threads_mutex.lock();
      auto k = temp->thread_id().lock();
      // adding object to thread with demanded id
      for (auto i = threads.begin(); i != threads.end(); i++) {
        if (*k == i->get()->thread_id()) {
          i->get()->AddObject(object);
          break;
        }
      }
      threads_mutex.unlock();
    }

    return 1;
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
    void ThreadFunction() {
      auto core = Core::GetInstance();
      while (true) {
        for (const auto& object : objects_) {
          object.lock()->UpdateExecutionTime(core->global_tick_);
        }
        if (local_tick_ % (core->tickrate_ / 10) == 0 &&
            !objects_to_add_.empty()) {
          std::scoped_lock<std::mutex> lock(objects_to_add_mutex_);
          objects_.insert(objects_.end(), objects_to_add_.begin(),
                          objects_to_add_.end());
          objects_to_add_.clear();
        }
        if (local_tick_ % (core->tickrate_ / 2) == 0) {
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
  std::thread::id operational_thread_id_;
  std::mutex thread_ready_counter_mutex_;
  int thread_ready_counter_ = -100000;
  void ThreadReady(std::thread::id id) {
    {
      std::scoped_lock<std::mutex> lock(thread_ready_counter_mutex_);
      thread_ready_counter_ += 1;
    }
    while (thread_ready_counter_ < 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if (operational_thread_id_ == id) {
      thread_ready_counter_ = -(int32_t)threads.size();
      global_tick_ += 1;
    }
  }

  Core() {
    for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
      auto ptr = std::make_unique<UpdateThread>(global_tick_);
      threads.push_back(std::move(ptr));
    }
    operational_thread_id_ = threads[0]->thread_id();
    while (thread_ready_counter_ != -100000 + threads.size()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::scoped_lock<std::mutex> lock(thread_ready_counter_mutex_);
    thread_ready_counter_ = 0;

    last_tick_timestamp = glfwGetTime();
  }

  static std::shared_ptr<Core> core_ptr_;

  double last_tick_timestamp = 0;
  uint64_t global_tick_ = 0;

  uint64_t tickrate_ = 0;

  std::vector<std::unique_ptr<UpdateThread>> threads;
  std::mutex threads_mutex;
};
}  // namespace engine::core