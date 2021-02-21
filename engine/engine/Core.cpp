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
}  // namespace engine::core