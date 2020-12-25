#pragma once
#include "Window.h"

Window::Window(int x, int y, std::string title, GLFWmonitor* monitor,
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
  glfwSetWindowMaximizeCallback(this->window_ptr_,
                                StaticMaximizeCallback);
  glfwSetFramebufferSizeCallback(this->window_ptr_,
                                 StaticFramebufferSizeCallback);
  glfwSetWindowContentScaleCallback(this->window_ptr_,
                                    StaticContentScaleCallback);
}

Window::~Window() { glfwDestroyWindow(window_ptr_); }

bool Window::Alive() { return window_ptr_ != NULL; }

bool Window::ShouldClose() const { return glfwWindowShouldClose(window_ptr_); }

void Window::SetWindowAttrib(int attrib, int value) {
  glfwSetWindowAttrib(window_ptr_, attrib, value);
}

void Window::SetWindowAttrib(glm::ivec2 attrib) {
  glfwSetWindowAttrib(window_ptr_, attrib.x, attrib.y);
}

void Window::MakeContextCurrent() { glfwMakeContextCurrent(window_ptr_); }

void Window::SwapBuffers() { glfwSwapBuffers(window_ptr_); }

void Window::PollEvents() { glfwPollEvents(); }

void Window::SwapInterval(int interval) { glfwSwapInterval(1); }

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

void Window::SetWindowTitle(const std::string title) {
  glfwSetWindowTitle(window_ptr_, title.c_str());
  this->title_ = title;
}

void Window::SetWindowIcon(const GLFWimage* images, int count) {
  glfwSetWindowIcon(window_ptr_, count, images);
}

GLFWmonitor* Window::GetMonitor() { return glfwGetWindowMonitor(window_ptr_); }

void Window::SetMonitor(GLFWmonitor* monitor, int xpos, int ypos,
                        int width, int height, int refresh_rate) {
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  if (width == height == 0) {
    width = mode->width;
    height = mode->height;
  }
  if (refresh_rate == 0) {
    refresh_rate = mode->refreshRate;
  }
  glfwSetWindowMonitor(window_ptr_, monitor, 0, 0, width, height, refresh_rate);
}

void Window::IconifyWindow() { glfwIconifyWindow(window_ptr_); }

void Window::MinimizeWindow() { glfwIconifyWindow(window_ptr_); }

void Window::MaximizeWindow() { glfwMaximizeWindow(window_ptr_); }

void Window::RestoreWindow() { glfwRestoreWindow(window_ptr_); }

void Window::HideWindow() { glfwHideWindow(window_ptr_); }

void Window::ShowWindow() { glfwShowWindow(window_ptr_); }

void Window::FocusWindow() { glfwFocusWindow(window_ptr_); }

void Window::RequestAttention() { glfwRequestWindowAttention(window_ptr_); }

void Window::PosCallback(int xpos, int ypos) {
  std::cout << "PosCallback: " << xpos << " " << ypos << std::endl;
}

void Window::SizeCallback(int width, int height) {
  std::cout << "SizeCallback: " << width << " " << height << std::endl;
}

void Window::CloseCallback() {
  std::cout << "CloseCallback: " << std::endl;
}

void Window::RefreshCallback() {
  std::cout << "RefreshCallback: " << std::endl;
}

void Window::FocusCallback(int focused) {
  std::cout << "FocusCallback: " << focused << std::endl;
}

void Window::IconifyCallback(int iconified) {
  std::cout << "IconifyCallback: " << iconified << std::endl;
}

void Window::MaximizeCallback(int maximized) {
  std::cout << "FocusCallback: " << maximized << std::endl;
}

void Window::FramebufferSizeCallback(int width, int height) {
  std::cout << "FramebufferSizeCallback: " << width << " " << height << std::endl;
}

void Window::ContentScaleCallback(float xscale, float yscale) {
  std::cout << "ContentScaleCallback: " << xscale << " " << yscale
            << std::endl;
}


void Window::StaticPosCallback(GLFWwindow* window, int xpos, int ypos) {
  ((Window*)(instances_.find(window))->second)->PosCallback(xpos, ypos);
}
void Window::StaticSizeCallback(GLFWwindow* window, int width, int height) {
  ((Window*)(instances_.find(window))->second)->SizeCallback(width, height);
}
void Window::StaticCloseCallback(GLFWwindow* window) {
  ((Window*)(instances_.find(window))->second)->CloseCallback();
}
void Window::StaticRefreshCallback(GLFWwindow* window) {
  ((Window*)(instances_.find(window))->second)->RefreshCallback();
}
void Window::StaticFocusCallback(GLFWwindow* window, int focused) {
  ((Window*)(instances_.find(window))->second)->FocusCallback(focused);
}
void Window::StaticIconifyCallback(GLFWwindow* window, int iconified) {
  ((Window*)(instances_.find(window))->second)->IconifyCallback(iconified);
}
void Window::StaticMaximizeCallback(GLFWwindow* window, int maximized) {
  ((Window*)(instances_.find(window))->second)->MaximizeCallback(maximized);
}
void Window::StaticFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  ((Window*)(instances_.find(window))->second)->FramebufferSizeCallback(width, height);
}
void Window::StaticContentScaleCallback(GLFWwindow* window, float xscale, float yscale) {
  ((Window*)(instances_.find(window))->second)->ContentScaleCallback(xscale, yscale);
}