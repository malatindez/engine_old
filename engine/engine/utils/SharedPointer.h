#include <shared_mutex>
#include <memory>
namespace engine::utils {
// Shared pointer with mutex which can be used between multiple threads.
template <class T>
class SharedPointer : public std::shared_ptr<T> final {
 public:
  
  void Destroy() noexcept { 
    std::unique_lock(mutex_);
    this->reset(nullptr);
  }

  bool Expired() const noexcept { return this->get() == nullptr; }

  std::shared_mutex mutex_;
};
}  // namespace engine::utils