#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "Input.h"
#pragma warning(disable : 26495)
// Functions from this class should be called only in the main thread.
namespace engine::core {
class Window : public Input {
 public:
  using PosFun = std::function<bool(int32_t, int32_t)>;
  using SizeFun = std::function<bool(int32_t, int32_t)>;
  using CloseFun = std::function<bool()>;
  using RefreshFun = std::function<bool()>;
  using FocusFun = std::function<bool(int32_t)>;
  using IconifyFun = std::function<bool(int32_t)>;
  using MaximizeFun = std::function<bool(int32_t)>;
  using FramebufferSizeFun = std::function<bool(int32_t, int32_t)>;
  using ContentScaleFun = std::function<bool(float, float)>;

  /* Disable copy and move semantics. */
  Window(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) = delete;

  Window(glm::ivec2 resolution, std::string const& title,
         GLFWmonitor* monitor = nullptr,
         GLFWwindow* share = nullptr)
      : Window(resolution.x, resolution.y, title, monitor, share) {}

  Window(int x, int y, std::string const& title, GLFWmonitor* monitor = nullptr,
         GLFWwindow* share = nullptr);

  ~Window() override { glfwDestroyWindow(window_ptr_); }

  [[nodiscard]] bool Alive() const noexcept;

  [[nodiscard]] bool ShouldClose() const;

  // This function sets the value of an attribute of the window.
  void SetWindowAttrib(int attrib, int value);
  // This function sets the value of an attribute of the window.
  void SetWindowAttrib(glm::ivec2 attrib);

  void MakeContextCurrent();

  // This function swaps the front and back buffers of the window when rendering
  // with OpenGL or OpenGL ES. If the swap interval is greater than zero, the
  // GPU driver waits the specified number of screen updates before swapping the
  // buffers.
  void SwapBuffers() const;

  // This function processes only those events that are already in the event
  // queue and then returns immediately. Processing events will cause the window
  // and input callbacks associated with those events to be called.
  void PollEvents() const;

  // This function sets the swap interval for the window, i.e. the number of
  // screen updates to wait from the time SwapBuffers was called before
  // swapping the buffers and returning. This is sometimes called vertical
  // synchronization, vertical retrace synchronization or just vsync.
  //
  // A context must be current on the calling thread. Calling this function
  // without a current context will cause a GLFW_NO_CURRENT_CONTEXT error.
  void SwapInterval(int interval) const;

  // Sets the size of a window in screen coordinates of the content
  // area or content area of the window The window system may impose
  // limits on window size.
  void SetWindowSize(const int x, const int y);

  // Sets the size of a window in screen coordinates of the content
  // area or content area of the window. The window system may impose
  // limits on window size.
  void SetWindowSize(const glm::ivec2 resolution);

  // The returned value is the current size of a window.
  [[nodiscard]] glm::ivec2 GetWindowSize() const;

  // The returned values are the distances, in screen coordinates,
  // from the edges of the content area to the corresponding edges of
  // the full window. As they are distances and not coordinates, they are
  // always zero or positive.
  [[nodiscard]] glm::ivec4 GetWindowFrameSize() const;

  // This function retrieves the content scale for the window. The
  // content scale is the ratio between the current DPI and the platform's
  // default DPI
  [[nodiscard]] glm::vec2 GetWindowContentScale() const;

  // The minimum and maximum size of the content area of a windowed mode window
  // can be enforced with glfwSetWindowSizeLimits. The user may resize the
  // window to any size and aspect ratio within the specified limits, unless the
  // aspect ratio is also set.
  //
  // To specify only a minimum size or only a maximum one, set the value to -1
  // or GLFW_DONT_CARE
  void SetWindowSizeLimits(const int min_width, const int min_height,
                           const int max_width, const int max_height);
  // The minimum and maximum size of the content area of a windowed mode window
  // can be enforced with glfwSetWindowSizeLimits. The user may resize the
  // window to any size and aspect ratio within the specified limits, unless the
  // aspect ratio is also set.
  //
  // To specify only a minimum size or only a maximum one, set the value to -1
  // or GLFW_DONT_CARE
  //
  // Values in the vector defined respectively:
  // x = min_width, y = min_height, z = max_width, w = max_height
  void SetWindowSizeLimits(const glm::ivec4 limits);

  // This function sets the required aspect ratio of the content area of the
  // specified window. If the window is full screen, the aspect ratio only takes
  // effect once it is made windowed. If the window is not resizable, this
  // function does nothing.
  //
  // The aspect ratio is specified as a numerator and a denominator and both
  // values must be greater than zero. For example, the common 16:9 aspect ratio
  // is specified as 16 and 9, respectively.
  //
  // If the numerator and denominator is set to GLFW_DONT_CARE or -1 then the
  // aspect ratio limit is disabled.
  void SetWindowAspectRatio(const int width, const int height);
  // This function sets the required aspect ratio of the content area of the
  // specified window. If the window is full screen, the aspect ratio only takes
  // effect once it is made windowed. If the window is not resizable, this
  // function does nothing.
  //
  // The aspect ratio is specified as a numerator and a denominator and both
  // values must be greater than zero. For example, the common 16:9 aspect ratio
  // is specified as 16 and 9, respectively.
  //
  // If the numerator and denominator is set to GLFW_DONT_CARE or -1 then the
  // aspect ratio limit is disabled.
  void SetWindowAspectRatio(const glm::ivec2 ratio);

  // This function sets the position, in screen coordinates, of the upper-left
  // corner of the content area of the window. If the window is a full screen
  // window, this function does nothing.
  void SetWindowPosition(const int x, const int y);
  // This function sets the position, in screen coordinates, of the upper-left
  // corner of the content area of the window. If the window is a full screen
  // window, this function does nothing.
  void SetWindowPosition(const glm::ivec2 pos);

  // This function retrieves the position, in screen coordinates, of the
  // upper-left corner of the content area of the window.
  glm::ivec2 GetWindowPosition() const;

  // This function sets the window title, encoded as UTF-8, of the specified
  // window.
  void SetWindowTitle(std::string const& title);

  // This function sets the icon of the specified window. If passed an array of
  // candidate images, those of or closest to the sizes desired by the system
  // are selected. If no images are specified, the window reverts to its default
  // icon.
  //
  // The pixels are 32 - bit, little - endian, non - premultiplied RGBA, i.e.
  // eight bits per channel with the red channel first. They are arranged
  // canonically as packed sequential rows, starting from the top - left corner.
  //
  // The desired image sizes varies depending on platform and system settings.
  // The selected images will be rescaled as needed. Good sizes include 16x16,
  // 32x32 and 48x48.
  //
  // To revert to the default window icon, pass in an empty image array:
  // glfwSetWindowIcon(window, 0, nullptr)
  void SetWindowIcon(const GLFWimage* images, int count);

  // This function returns the handle of the monitor that the specified window
  // is in full screen on.
  [[nodiscard]] GLFWmonitor* GetMonitor();

  // This function sets the monitor that the window uses for full screen mode
  // or, if the monitor is nullptr, makes it windowed mode.
  //
  // When setting a monitor, this function updates the width, height and refresh
  // rate of the desired video mode and switches to the video mode closest to
  // it. The window position is ignored when setting a monitor.
  //
  // When the monitor is nullptr, the position, width and height are used to place
  // the window content area. The refresh rate is ignored when no monitor is
  // specified.
  //
  // When a window transitions from full screen to windowed mode, this function
  // restores any previous window settings such as whether it is decorated,
  // floating, resizable, has size or aspect ratio limits, etc.
  void SetMonitor(GLFWmonitor* monitor = nullptr, int xpos = 0, int ypos = 0,
                  int width = 0, int height = 0, int refresh_rate = 0);

  // This function iconifies (minimizes) the specified window if it was
  // previously restored. If the window is already iconified, this function does
  // nothing.
  void IconifyWindow();
  // This function iconifies (minimizes) the specified window if it was
  // previously restored. If the window is already iconified, this function does
  // nothing.
  void MinimizeWindow();

  // This function maximizes the specified window if it was previously not
  // maximized. If the window is already maximized, this function does nothing.
  void MaximizeWindow();

  // This function restores the specified window if it was previously iconified
  // (minimized) or maximized. If the window is already restored, this function
  // does nothing.
  void RestoreWindow();

  // This function hides the specified window if it was previously visible. If
  // the window is already hidden or is in full screen mode, this function does
  // nothing.
  void HideWindow();

  // This function makes the specified window visible if it was previously
  // hidden. If the window is already visible or is in full screen mode, this
  // function does nothing.
  //
  // By default, windowed mode windows are focused when shown Set the
  // GLFW_FOCUS_ON_SHOW window hint to change this behavior for all newly
  // created windows, or change the behavior for an existing window with
  // SetWindowAttrib.
  void ShowWindow();

  // This function brings the specified window to front and sets input focus.
  // The window should already be visible and not iconified.
  //
  // By default, both windowed and full screen mode windows are focused when
  // initially created. Set the GLFW_FOCUSED to disable this behavior.
  //
  // Also by default, windowed mode windows are focused when shown with
  // glfwShowWindow. Set the GLFW_FOCUS_ON_SHOW to disable this behavior.
  void FocusWindow();

  // This function requests user attention to the specified window. On platforms
  // where this is not supported, attention is requested to the application as a
  // whole.
  //
  // Once the user has given attention, usually by focusing the window or
  // application, the system will end the request automatically.
  void RequestAttention();

  // The function you provide is the key position callback, which is called when
  // the window is moved. The callback is provided with the position, in screen
  // coordinates, of the upper-left corner of the content area of the window.
  void PushPosCallback(const std::shared_ptr<PosFun> ptr) noexcept;
  void PushSizeCallback(const std::shared_ptr<SizeFun> ptr) noexcept;
  void PushCloseCallback(const std::shared_ptr<CloseFun> ptr) noexcept;
  void PushRefreshCallback(const std::shared_ptr<RefreshFun> ptr) noexcept;
  void PushFocusCallback(const std::shared_ptr<FocusFun> ptr) noexcept;
  void PushIconifyCallback(const std::shared_ptr<IconifyFun> ptr) noexcept;
  void PushMaximizeCallback(const std::shared_ptr<MaximizeFun> ptr) noexcept;
  void PushFramebufferSizeCallback(
      const std::shared_ptr<FramebufferSizeFun> ptr) noexcept;
  void PushContentScaleCallback(
      const std::shared_ptr<ContentScaleFun> ptr) noexcept;

 protected:
  void PosCallback(int xpos, int ypos);

  // This function is the size callback, which is called when the window is
  // resized. The callback is provided with the size, in screen coordinates, of
  // the content area of the window.
  void SizeCallback(int width, int height);

  // This function is the close callback, which is called when the user attempts
  // to close the window, for example by clicking the close widget in the title
  // bar.
  void CloseCallback();

  // This function is the refresh callback, which is called when the content
  // area of the window needs to be redrawn, for example if the window has been
  // exposed after having been covered by another window.
  void RefreshCallback();

  // This function is the focus callback, which is called when the window gains
  // or loses input focus.
  void FocusCallback(int focused);

  // This function is the iconification callback, which is called when the
  // window is iconified or restored.
  void IconifyCallback(int iconified);

  // This function is the maximization callback, which is called when the window
  // is maximized or restored.
  void MaximizeCallback(int maximized);

  // This function is the framebuffer resize callback, which is called when the
  // framebuffer of the specified window is resized.
  void FramebufferSizeCallback(int width, int height);

  // This function is the window content scale callback, which is called when
  // the content scale of the specified window changes.
  void ContentScaleCallback(float xscale, float yscale);

 private:
  static void StaticPosCallback(GLFWwindow* window, int xpos, int ypos);
  static void StaticSizeCallback(GLFWwindow* window, int width, int height);
  static void StaticCloseCallback(GLFWwindow* window);
  static void StaticRefreshCallback(GLFWwindow* window);
  static void StaticFocusCallback(GLFWwindow* window, int focused);
  static void StaticIconifyCallback(GLFWwindow* window, int iconified);
  static void StaticMaximizeCallback(GLFWwindow* window, int maximized);
  static void StaticFramebufferSizeCallback(GLFWwindow* window, int width,
                                            int height);
  static void StaticContentScaleCallback(GLFWwindow* window, float xscale,
                                         float yscale);

  std::stack<std::shared_ptr<PosFun>> pos_callbacks_;
  std::stack<std::shared_ptr<SizeFun>> size_callbacks_;
  std::stack<std::shared_ptr<CloseFun>> close_callbacks_;
  std::stack<std::shared_ptr<RefreshFun>> refresh_callbacks_;
  std::stack<std::shared_ptr<FocusFun>> focus_callbacks_;
  std::stack<std::shared_ptr<IconifyFun>> iconify_callbacks_;
  std::stack<std::shared_ptr<MaximizeFun>> maximize_callbacks_;
  std::stack<std::shared_ptr<FramebufferSizeFun>> framebuffer_size_callbacks_;
  std::stack<std::shared_ptr<ContentScaleFun>> content_scale_callbacks_;

  std::string title_;
  GLFWwindow* window_ptr_;
};
}  // namespace engine::core
#pragma warning(default : 26495)