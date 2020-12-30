#pragma once
#include <thread>
namespace engine::core {
class Ticker {
 public:
  Ticker(uint32_t tickrate, std::thread::id &thread_id)
      : tickrate_(tickrate),
        thread_id_(std::make_shared<std::thread::id>(thread_id)) {}

  explicit Ticker(uint32_t tickrate) : tickrate_(tickrate) {}

  virtual ~Ticker() = default;

  virtual void Update(const unsigned int tick, const float timeDelta) {
    // Intentionally unimplemented
  }

  uint32_t tickrate() const noexcept { return tickrate_; }

  std::weak_ptr<std::thread::id> thread_id() const {
    return std::weak_ptr<std::thread::id>(thread_id_);
  }

 protected:
  void SetTickrate(uint32_t tickrate) { tickrate_ = tickrate; }
  void SetThreadID(std::thread::id &id) {
    thread_id_ = std::make_shared<std::thread::id>(id);
  }
 private:
  uint32_t tickrate_;
  std::shared_ptr<std::thread::id> thread_id_ = std::shared_ptr<std::thread::id>(nullptr);
};
}  // namespace engine::core