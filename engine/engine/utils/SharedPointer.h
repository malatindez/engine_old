#include <shared_mutex>
#include <memory>
namespace engine::utils {
// Shared pointer with mutex which can be used between multiple threads.
// To write changes to the object you should call unique_lock on mutex_
// To read the object data, you should call shared_lock on mutex_
// After locking the object you should call Expired() method to ensure that the
// object is still valid. After that you can continue the work with the object.
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