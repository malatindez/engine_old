#pragma once

#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <future>

#include <engine/client/misc/Window.h>
#include <engine/client/render/Shader.h>
#include <engine/Config.h>

namespace engine::client::render {

  class RenderCore {
  /* Disable copy and move semantics. */
  RenderCore(const RenderCore&) = delete;
  RenderCore(RenderCore&&) = delete;
  RenderCore& operator=(const RenderCore&) = delete;
  RenderCore& operator=(RenderCore&&) = delete;

  [[nodiscard]] static std::shared_ptr<RenderCore> GetInstance() noexcept {
    if (render_ptr_ == nullptr) {
      if (render_creation_mutex_.try_lock()) {
        render_ptr_ = std::shared_ptr<RenderCore>(new RenderCore());
        render_creation_mutex_.unlock();
      } else {
        std::scoped_lock<std::mutex> lock(render_creation_mutex_);
      }
    }
    return render_ptr_;
  }

  std::weak_ptr<Window> main_window() const noexcept { 
    return main_window_;
  }

  private:
  RenderCore() {
    main_render_thread_ =
        std::make_unique<std::thread>(&RenderCore::Update, this);
    auto window = std::make_shared<engine::client::Window>(
        1366, 768, "engine " + std::string(kEngineVersion), nullptr, nullptr);
  }

  [[noreturn]] void Update() const {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                   core::Config::OGL_version_major());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                   core::Config::OGL_version_minor());
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    while (true) {
    }
  }

  std::unique_ptr<std::thread> main_render_thread_;

  std::shared_ptr<Window> main_window_;

  static std::mutex render_creation_mutex_;
  static std::shared_ptr<RenderCore> render_ptr_;
};
}