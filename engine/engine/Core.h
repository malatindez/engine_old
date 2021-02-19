#pragma once
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
        if ((*i).get()->exec_time < min_execution_thread->get()->exec_time) {
          min_execution_thread = i;
        }
      }
      std::scoped_lock<std::mutex> lock_(
          min_execution_thread->get()->ticking_objects_mutex);
      min_execution_thread->get()->ticking_objects.push_back(object);
    } else {
      threads_mutex.lock();
      auto k = temp->thread_id().lock();
      // adding object to thread with demanded id
      for (auto i = threads.begin(); i != threads.end(); i++) {
        if (*k == i->get()->thread.get_id()) {
          std::scoped_lock<std::mutex> lock(i->get()->ticking_objects_mutex);
          (*i).get()->ticking_objects.push_back(object);
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
    [[nodiscard]] double exec_time() const noexcept { return exec_time_; }

    void AddObject(std::weak_ptr<Ticker> object) {
      std::scoped_lock<std::mutex> lock(objects_mutex_);
      objects_.push_back(object);
      optimize_ = true;
    }

   private:

     struct ExecutionList {
       using ObjectIterator = std::vector<std::weak_ptr<Ticker>>::iterator;
       std::vector<ObjectIterator> objects;
       double execution_time;
     };

    void ThreadFunction() {
      auto core = Core::GetInstance();
      while (true) {
        auto exec_list =
            optimized_queue_.begin() + local_tick_ % optimized_queue_.size();
        for (const auto& object : exec_list->objects) {
            object->lock()->UpdateExecutionTime(core->global_tick_);
        }

        // update exec_list execution_time once per 60 ticks
        if ((local_tick_ + local_tick_ % optimized_queue_.size()) % 60 == 0) {
          exec_list->execution_time = 0;
          for (auto itr = objects_.begin();
               itr != objects_.end(); itr++) {
            exec_list->execution_time += itr->lock()->average_update_time();
          }
        }

        if (optimize_) {
          OptimizeQueue();
        }
      }
    }
    

    void OptimizeQueue() { 
      std::scoped_lock<std::mutex> lock(objects_mutex_);
      std::map<uint32_t, std::vector<ExecutionList::ObjectIterator>> map;
      for (auto itr = objects_.begin(); itr != objects_.end(); itr++) {
        uint32_t tickrate = itr->lock()->tickrate();
        if (map.count(tickrate)) {
          map[tickrate].push_back(itr);
        } else {
          map[tickrate] = std::vector<ExecutionList::ObjectIterator>();
          map[tickrate].push_back(itr);
        }
      }
      optimized_queue_.clear();
      uint32_t max_tickrate = 0;
      for (auto itr = map.begin(); itr != map.end(); itr++) {
        max_tickrate = max(itr->first, max_tickrate);
      }
    }

    // Optimization flag, if set to true, optimize_queue will be called.
    bool optimize_;

    std::vector<ExecutionList> optimized_queue_;

    std::mutex objects_mutex_;
    std::vector<std::weak_ptr<Ticker>> objects_;



    std::unique_ptr<std::thread> thread_;

    // stores current local tick
    uint64_t local_tick_;

    double exec_time_ = 0;
  };

  Core() {
    for (int i = 0; i < std::thread::hardware_concurrency(); i++) {
      auto ptr = std::make_unique<ThreadData>();
      ptr->thread = std::make_unique<std::thread>(
          std::thread(&Core::Updater, this, ptr.get()));
      ptr->local_tick = 0;
      ptr->exec_time = 0;
    }
    last_tick_timestamp = glfwGetTime();
  }

  static std::shared_ptr<Core> core_ptr_;

  double last_tick_timestamp = 0;
  uint64_t global_tick_ = 0;

  uint64_t tickrate_ = 0;

  std::vector<std::unique_ptr<ThreadData>> threads;
  std::mutex threads_mutex;
};
}  // namespace engine::core