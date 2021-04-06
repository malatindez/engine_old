
#include <memory>
#include <mutex>

#include "EngineConfig.h"

namespace engine::core {
class Config {
 public:
  /* Disable copy and move semantics. */
  Config(const Config&) = delete;
  Config(Config&&) = delete;
  Config& operator=(const Config&) = delete;
  Config& operator=(Config&&) = delete;

  // returns shared pointer to the Config.
  [[nodiscard]] static std::shared_ptr<Config> GetInstance() noexcept {
    if (config_ptr_ == nullptr) {
      if (config_creation_mutex_.try_lock()) {
        config_ptr_ = std::shared_ptr<Config>(new Config());
        config_creation_mutex_.unlock();
      } else {
        std::scoped_lock<std::mutex> lock(config_creation_mutex_);
      }
    }
    return config_ptr_;
  }

  static std::string_view version() inline noexcept { 
    return kEngineVersion; 
  }

  static std::string_view version_major() inline noexcept {
    return kEngineVersionMajor;
  }

  static std::string_view version_minor() inline noexcept {
    return kEngineVersionMinor;
  }

  static std::string_view version_patch() inline noexcept {
    return kEngineVersionPatch;
  }

  static uint32_t OGL_version_major() inline noexcept {
    return kOpenGLVersionMajor;
  }

  static uint32_t OGL_version_minor() inline noexcept {
    return kOpenGLVersionMinor;
  }

 private:
  Config() {}

  static std::mutex config_creation_mutex_;
  static std::shared_ptr<Config> config_ptr_;
};
}  // namespace engine::core