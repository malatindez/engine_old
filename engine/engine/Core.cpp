#include "Core.h"
namespace engine::core {

std::shared_ptr<Core> Core::core_ptr_ = std::shared_ptr<Core>(nullptr);
std::mutex Core::core_creation_mutex_;

std::shared_ptr<Core> Core::GetInstance() noexcept {
  if (core_ptr_ == nullptr) {
    if (core_creation_mutex_.try_lock()) {
      core_ptr_ = std::shared_ptr<Core>(new Core());
      core_creation_mutex_.unlock();
    } else {
      core_creation_mutex_.lock();
      core_creation_mutex_.unlock();
    }
  }
  return core_ptr_;
}

double Core::time() {
  std::chrono::time_point<std::chrono::system_clock> now =
      std::chrono::system_clock::now();
  double microseconds =
      (double)std::chrono::duration_cast<std::chrono::microseconds>(
          now.time_since_epoch())
          .count();
  return microseconds / 1e6;
}

uint64_t Core::global_tick() noexcept { return core_ptr_->global_tick_; }

uint32_t Core::CalcTickrate(uint32_t times_per_second) {
  return (uint32_t)ceil(double(core_ptr_->tickrate_) / times_per_second);
}

double Core::tick_delta() { return core_ptr_->last_tick_timedelta_; }

int Core::AddTickingObject(std::weak_ptr<Ticker> object) {
  auto temp = object.lock();
  if (temp == nullptr || threads_.empty()) {
    return 0;
  }
  std::scoped_lock<std::mutex> lock(threads_mutex_);

  // add object thread with desired id
  if (!temp->thread_id().expired()) {
    auto k = temp->thread_id().lock();

    auto t = std::find_if(std::begin(threads_), std::end(threads_),
                          [&k](std::unique_ptr<UpdateThread> const& thread) {
                            return *k == thread->thread_id();
                          });
    t->get()->AddObject(object);
    return 1;
  }

  // search for the thread with minimum execution time
  auto min_execution_thread = threads_.begin();
  for (auto i = threads_.begin() + 1; i != threads_.end(); i++) {
    if ((*i).get()->exec_time() < min_execution_thread->get()->exec_time()) {
      min_execution_thread = i;
    }
  }
  min_execution_thread->get()->AddObject(object);

  return 1;
}

std::chrono::nanoseconds Core::calc_overhead() {
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

inline void Core::busy_sleep(std::chrono::nanoseconds t) const {
  auto end = std::chrono::steady_clock::now() + t - overhead_;
  while (std::chrono::steady_clock::now() < end)
    ;
}

void Core::ThreadReady(std::thread::id id) {
  std::unique_lock lock(sync_mutex_);

  if (operational_thread_id_ != id) {
    sync_threads_ += 1;
    sync_var_.wait(lock);
  } else {
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
  }
}

Core::Core() {
  last_tick_timestamp_ = time();
  for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
    auto ptr = std::make_unique<UpdateThread>(global_tick_);
    threads_.push_back(std::move(ptr));
  }
  operational_thread_id_ = threads_[0]->thread_id();
}

Core::UpdateThread::UpdateThread(uint32_t tick) : local_tick_(tick) {
  this->thread_ =
      std::make_unique<std::thread>(&UpdateThread::ThreadFunction, this);
}
Core::UpdateThread::~UpdateThread() {
  die_ = true;
  thread_->join();
}
double Core::UpdateThread::exec_time() const noexcept { return exec_time_; }

void Core::UpdateThread::AddObject(std::weak_ptr<Ticker> object) {
  std::scoped_lock<std::mutex> lock(objects_to_add_mutex_);
  objects_to_add_.push_back(object);
}

std::thread::id Core::UpdateThread::thread_id() const noexcept {
  return thread_->get_id();
}

#ifdef _MSC_VER
#define malatindez_FORCE_INLINE [[msvc::forceinline]]
#else
#define malatindez_FORCE_INLINE __attribute__((always_inline))
#endif

void Core::UpdateThread::ThreadFunction() {
  std::shared_ptr<Core> core = Core::GetInstance();
  core->ThreadReady(thread_->get_id());
  auto update_objects = [this, &core]() malatindez_FORCE_INLINE {
    for (auto itr = objects_.begin(); itr != objects_.end();) {
      if (itr->expired()) {  // if object is expired, remove it from objects_
        itr = objects_.erase(itr);
      } else {  // update non-expired object
        itr->lock()->UpdateExecutionTime(core->global_tick_);
        itr++;
      }
    }
  };

  // 16 times per second
  auto add_objects_tickrate = (uint32_t)ceil((double)core->tickrate_ / 16);
  // 4 times per second
  auto update_exec_time_tickrate = (uint32_t)ceil((double)core->tickrate_ / 4);

  auto exec_time_accumulate_ =
      [](double a, std::weak_ptr<Ticker> object) malatindez_FORCE_INLINE {
        return a +
               object.lock()->average_update_time() / object.lock()->tickrate();
      };

  while (!die_) {
    update_objects();

    // add objects 16 times per second
    if (local_tick_ % add_objects_tickrate == 0 && !objects_to_add_.empty()) {
      std::scoped_lock<std::mutex> lock(objects_to_add_mutex_);
      objects_.insert(objects_.end(), objects_to_add_.begin(),
                      objects_to_add_.end());
      objects_to_add_.clear();
    }

    if (local_tick_ % update_exec_time_tickrate == 0) {
      exec_time_ = std::accumulate(std::begin(objects_), std::end(objects_),
                                   0.0, exec_time_accumulate_);
      exec_time_ *= core->tickrate_;
    }
    core->ThreadReady(thread_->get_id());
    local_tick_ = core->global_tick_;
  }
}
}  // namespace engine::core