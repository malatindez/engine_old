#include "Core.h"
namespace engine::core {

std::shared_ptr<Core> Core::core_ptr_ = std::shared_ptr<Core>(nullptr);

std::shared_ptr<Core> Core::GetInstance() noexcept {
  if (core_ptr_ == nullptr) {
    core_ptr_ = std::shared_ptr<Core>(new Core());
  }
  return core_ptr_;
}
}