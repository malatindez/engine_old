#pragma once
#include "Window.h"

namespace engine::core {
Window::Window(int x, int y, std::string const& title, GLFWmonitor* monitor,
               GLFWwindow* share)
    : title_(title) {
  window_ptr_ = glfwCreateWindow(x, y, title.c_str(), monitor, share);
  this->init();

  glfwSetWindowPosCallback(this->window_ptr_, StaticPosCallback);
  glfwSetWindowSizeCallback(this->window_ptr_, StaticSizeCallback);
  glfwSetWindowCloseCallback(this->window_ptr_, StaticCloseCallback);
  glfwSetWindowRefreshCallback(this->window_ptr_, StaticRefreshCallback);
  glfwSetWindowFocusCallback(this->window_ptr_, StaticFocusCallback);
  glfwSetWindowIconifyCallback(this->window_ptr_, StaticIconifyCallback);
  glfwSetWindowMaximizeCallback(this->window_ptr_, StaticMaximizeCallback);
  glfwSetFramebufferSizeCallback(this->window_ptr_,
                                 StaticFramebufferSizeCallback);
  glfwSetWindowContentScaleCallback(this->window_ptr_,
                                    StaticContentScaleCallback);
}

bool Window::Alive() const noexcept { return window_ptr_ != nullptr; }

bool Window::ShouldClose() const { return glfwWindowShouldClose(window_ptr_); }

void Window::SetWindowAttrib(int attrib, int value) {
  glfwSetWindowAttrib(window_ptr_, attrib, value);
}

void Window::SetWindowAttrib(glm::ivec2 attrib) {
  glfwSetWindowAttrib(window_ptr_, attrib.x, attrib.y);
}

void Window::MakeContextCurrent() { glfwMakeContextCurrent(window_ptr_); }

void Window::SwapBuffers() const { glfwSwapBuffers(window_ptr_); }

void Window::PollEvents() const{ glfwPollEvents(); }

void Window::SwapInterval(int interval) const { glfwSwapInterval(interval); }

void Window::SetWindowSize(const int x, const int y) {
  glfwSetWindowSize(window_ptr_, x, y);
}

void Window::SetWindowSize(const glm::ivec2 resolution) {
  glfwSetWindowSize(window_ptr_, resolution.x, resolution.y);
}

glm::ivec2 Window::GetWindowSize() const {
  int width, height;
  glfwGetWindowSize(window_ptr_, &width, &height);
  return glm::ivec2(width, height);
}

glm::ivec4 Window::GetWindowFrameSize() const {
  int left, top, right, bottom;
  glfwGetWindowFrameSize(window_ptr_, &left, &top, &right, &bottom);
  return glm::ivec4(left, top, right, bottom);
}

glm::vec2 Window::GetWindowContentScale() const {
  float xscale, yscale;
  glfwGetWindowContentScale(window_ptr_, &xscale, &yscale);
  return glm::vec2(xscale, yscale);
}

void Window::SetWindowSizeLimits(const int min_width, const int min_height,
                                 const int max_width, const int max_height) {
  glfwSetWindowSizeLimits(window_ptr_, min_width, min_height, max_width,
                          max_height);
}

void Window::SetWindowSizeLimits(const glm::ivec4 limits) {
  glfwSetWindowSizeLimits(window_ptr_, limits.x, limits.y, limits.z, limits.w);
}

void Window::SetWindowAspectRatio(const int width, const int height) {
  glfwSetWindowAspectRatio(window_ptr_, width, height);
}

void Window::SetWindowAspectRatio(const glm::ivec2 ratio) {
  glfwSetWindowAspectRatio(window_ptr_, ratio.x, ratio.y);
}

void Window::SetWindowPosition(const int x, const int y) {
  glfwSetWindowPos(window_ptr_, x, y);
}

void Window::SetWindowPosition(const glm::ivec2 pos) {
  glfwSetWindowPos(window_ptr_, pos.x, pos.y);
}

glm::ivec2 Window::GetWindowPosition() const {
  int xpos, ypos;
  glfwGetWindowPos(window_ptr_, &xpos, &ypos);
  return glm::ivec2(xpos, ypos);
}

void Window::SetWindowTitle(std::string const& title) {
  glfwSetWindowTitle(window_ptr_, title.c_str());
  this->title_ = std::string(title);
}

void Window::SetWindowIcon(const GLFWimage* images, int count) {
  glfwSetWindowIcon(window_ptr_, count, images);
}

GLFWmonitor* Window::GetMonitor() { return glfwGetWindowMonitor(window_ptr_); }

void Window::SetMonitor(GLFWmonitor* monitor, int xpos, int ypos, int width,
                        int height, int refresh_rate) {
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  if (width == height == 0) {
    width = mode->width;
    height = mode->height;
  }
  if (refresh_rate == 0) {
    refresh_rate = mode->refreshRate;
  }
  glfwSetWindowMonitor(window_ptr_, monitor, xpos, ypos, width, height, refresh_rate);
}

void Window::IconifyWindow() { glfwIconifyWindow(window_ptr_); }

void Window::MinimizeWindow() { glfwIconifyWindow(window_ptr_); }

void Window::MaximizeWindow() { glfwMaximizeWindow(window_ptr_); }

void Window::RestoreWindow() { glfwRestoreWindow(window_ptr_); }

void Window::HideWindow() { glfwHideWindow(window_ptr_); }

void Window::ShowWindow() { glfwShowWindow(window_ptr_); }

void Window::FocusWindow() { glfwFocusWindow(window_ptr_); }

void Window::RequestAttention() { glfwRequestWindowAttention(window_ptr_); }

void Window::PushPosCallback(const std::shared_ptr<PosFun> ptr) noexcept {
  pos_callbacks_.push(ptr);
}
void Window::PushSizeCallback(const std::shared_ptr<SizeFun> ptr) noexcept {
  pos_callbacks_.push(ptr);
}
void Window::PushCloseCallback(const std::shared_ptr<CloseFun> ptr) noexcept {
  close_callbacks_.push(ptr);
}
void Window::PushRefreshCallback(
    const std::shared_ptr<RefreshFun> ptr) noexcept {
  refresh_callbacks_.push(ptr);
}
void Window::PushFocusCallback(const std::shared_ptr<FocusFun> ptr) noexcept {
  focus_callbacks_.push(ptr);
}
void Window::PushIconifyCallback(
    const std::shared_ptr<IconifyFun> ptr) noexcept {
  iconify_callbacks_.push(ptr);
}
void Window::PushMaximizeCallback(
    const std::shared_ptr<MaximizeFun> ptr) noexcept {
  maximize_callbacks_.push(ptr);
}
void Window::PushFramebufferSizeCallback(
    const std::shared_ptr<FramebufferSizeFun> ptr) noexcept {
  framebuffer_size_callbacks_.push(ptr);
}
void Window::PushContentScaleCallback(
    const std::shared_ptr<ContentScaleFun> ptr) noexcept {
  content_scale_callbacks_.push(ptr);
}

void Window::PosCallback(int xpos, int ypos) {
  std::cout << "PosCallback: " << xpos << " " << ypos << std::endl;
  while (!pos_callbacks_.empty()) {
    if (pos_callbacks_.top().use_count() == 1) {
      pos_callbacks_.pop();
    } else {
      if ((*pos_callbacks_.top())(xpos, ypos)) {
        pos_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::SizeCallback(int width, int height) {
  std::cout << "SizeCallback: " << width << " " << height << std::endl;
  while (!size_callbacks_.empty()) {
    if (size_callbacks_.top().use_count() == 1) {
      size_callbacks_.pop();
    } else {
      if ((*size_callbacks_.top())(width, height)) {
        size_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::CloseCallback() {
  std::cout << "CloseCallback: " << std::endl;
  while (!close_callbacks_.empty()) {
    if (close_callbacks_.top().use_count() == 1) {
      close_callbacks_.pop();
    } else {
      if ((*close_callbacks_.top())()) {
        close_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::RefreshCallback() {
  std::cout << "RefreshCallback: " << std::endl;
  while (!refresh_callbacks_.empty()) {
    if (refresh_callbacks_.top().use_count() == 1) {
      refresh_callbacks_.pop();
    } else {
      if ((*refresh_callbacks_.top())()) {
        refresh_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::FocusCallback(int focused) {
  std::cout << "FocusCallback: " << focused << std::endl;
  while (!focus_callbacks_.empty()) {
    if (focus_callbacks_.top().use_count() == 1) {
      focus_callbacks_.pop();
    } else {
      if ((*focus_callbacks_.top())(focused)) {
        focus_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::IconifyCallback(int iconified) {
  std::cout << "IconifyCallback: " << iconified << std::endl;
  while (!iconify_callbacks_.empty()) {
    if (iconify_callbacks_.top().use_count() == 1) {
      iconify_callbacks_.pop();
    } else {
      if ((*iconify_callbacks_.top())(iconified)) {
        iconify_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::MaximizeCallback(int maximized) {
  std::cout << "FocusCallback: " << maximized << std::endl;
  while (!maximize_callbacks_.empty()) {
    if (maximize_callbacks_.top().use_count() == 1) {
      maximize_callbacks_.pop();
    } else {
      if ((*maximize_callbacks_.top())(maximized)) {
        maximize_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::FramebufferSizeCallback(int width, int height) {
  std::cout << "FramebufferSizeCallback: " << width << " " << height
            << std::endl;
  while (!framebuffer_size_callbacks_.empty()) {
    if (framebuffer_size_callbacks_.top().use_count() == 1) {
      framebuffer_size_callbacks_.pop();
    } else {
      if ((*framebuffer_size_callbacks_.top())(width, height)) {
        framebuffer_size_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::ContentScaleCallback(float xscale, float yscale) {
  std::cout << "ContentScaleCallback: " << xscale << " " << yscale << std::endl;
  while (!content_scale_callbacks_.empty()) {
    if (content_scale_callbacks_.top().use_count() == 1) {
      content_scale_callbacks_.pop();
    } else {
      if ((*content_scale_callbacks_.top())(xscale, yscale)) {
        content_scale_callbacks_.pop();
      }
      break;
    }
  }
}

void Window::StaticPosCallback(GLFWwindow* window, int xpos, int ypos) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->PosCallback(xpos, ypos);
  }
}
void Window::StaticSizeCallback(GLFWwindow* window, int width, int height) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->SizeCallback(width, height);
  }
}
void Window::StaticCloseCallback(GLFWwindow* window) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->CloseCallback();
  }
}
void Window::StaticRefreshCallback(GLFWwindow* window) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->RefreshCallback();
  }
}
void Window::StaticFocusCallback(GLFWwindow* window, int focused) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->FocusCallback(focused);
  }
}
void Window::StaticIconifyCallback(GLFWwindow* window, int iconified) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->IconifyCallback(iconified);
  }
}
void Window::StaticMaximizeCallback(GLFWwindow* window, int maximized) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->MaximizeCallback(maximized);
  }
}
void Window::StaticFramebufferSizeCallback(GLFWwindow* window, int width,
                                           int height) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->FramebufferSizeCallback(width, height);
  }
}
void Window::StaticContentScaleCallback(GLFWwindow* window, float xscale,
                                        float yscale) {
  Window* casted_window = static_cast<Window*>(instances_.find(window)->second);
  if (casted_window != nullptr) {
    casted_window->ContentScaleCallback(xscale, yscale);
  }
}
}  // namespace engine::core