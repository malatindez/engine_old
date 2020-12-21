#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#pragma warning(disable : 26495)
// Functions from this class should be called only in main thread
class Window {
  GLFWwindow* windowPtr = nullptr;
  std::string title;

 public:
  // hint class, heir of glm::ivec2
  // this class is used to store glfwWindowHint values
  class Hint : glm::ivec2 {
   public:
    Hint(const int hint, const int value) : glm::ivec2(hint, value) {}
    Hint(const glm::ivec2 vec) : glm::ivec2(vec) {}
    int GetHint() const noexcept { return this->x; }
    int GetValue() const noexcept { return this->y; }
    void Apply() const noexcept { glfwWindowHint(this->x, this->y); }
  };

  GLFWwindow* GetPointer() const noexcept { return windowPtr; }

  Window(std::vector<Hint> hints, glm::ivec2 resolution, std::string title,
         GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL)
      : title(title) {
    for (int i = 0; i < hints.size(); i++) {
      hints[i].Apply();
    }
    init(resolution, title, monitor, share);
  }

  ~Window() { glfwDestroyWindow(windowPtr); }

  bool ShouldClose() const { return glfwWindowShouldClose(windowPtr); }
  // This function sets the value of an attribute of the specified window.
  void SetWindowAttrib(int attrib, int value) {
    glfwSetWindowAttrib(windowPtr, attrib, value);
  }
  // This function sets the value of an attribute of the specified window.
  void SetWindowAttrib(glm::ivec2 attrib) {
    glfwSetWindowAttrib(windowPtr, attrib.x, attrib.y);
  }

  void MakeContextCurrent() { glfwMakeContextCurrent(windowPtr); }

  // This function swaps the front and back buffers of the window when rendering
  // with OpenGL or OpenGL ES. If the swap interval is greater than zero, the
  // GPU driver waits the specified number of screen updates before swapping the
  // buffers.
  void SwapBuffers() { glfwSwapBuffers(windowPtr); }

  // This function sets the swap interval for the window, i.e. the number of
  // screen updates to wait from the time SwapBuffers was called before
  // swapping the buffers and returning. This is sometimes called vertical
  // synchronization, vertical retrace synchronization or just vsync.
  //
  // A context must be current on the calling thread. Calling this function
  // without a current context will cause a GLFW_NO_CURRENT_CONTEXT error.
  void SwapInterval(int interval) { glfwSwapInterval(1); }

  // Sets the size of a window in screen coordinates of the content
  // area or content area of the window The window system may impose
  // limits on window size.

  void SetWindowSize(const int x, const int y) {
    glfwSetWindowSize(windowPtr, x, y);
  }

  // Sets the size of a window in screen coordinates of the content
  // area or content area of the window. The window system may impose
  // limits on window size.
  void SetWindowSize(const glm::ivec2 resolution) {
    glfwSetWindowSize(windowPtr, resolution.x, resolution.y);
  }

  /*
          The returned value is the current size of a window.
  */
  glm::ivec2 GetWindowSize() const {
    int width, height;
    glfwGetWindowSize(windowPtr, &width, &height);
    return glm::ivec2(width, height);
  }

  // The returned values are the distances, in screen coordinates,
  // from the edges of the content area to the corresponding edges of
  // the full window. As they are distances and not coordinates, they are
  // always zero or positive.
  glm::ivec4 GetWindowFrameSize() const {
    int left, top, right, bottom;
    glfwGetWindowFrameSize(windowPtr, &left, &top, &right, &bottom);
    return glm::ivec4(left, top, right, bottom);
  }
  // This function retrieves the content scale for the window. The
  // content scale is the ratio between the current DPI and the platform's
  // default DPI
  glm::vec2 GetWindowContentScale() const {
    float xscale, yscale;
    glfwGetWindowContentScale(windowPtr, &xscale, &yscale);
  }

  // The minimum and maximum size of the content area of a windowed mode window
  // can be enforced with glfwSetWindowSizeLimits. The user may resize the
  // window to any size and aspect ratio within the specified limits, unless the
  // aspect ratio is also set.
  //
  // To specify only a minimum size or only a maximum one, set the value to -1
  // or GLFW_DONT_CARE
  void SetWindowSizeLimits(const int minwidth, const int minheight,
                           const int maxwidth, const int maxheight) {
    glfwSetWindowSizeLimits(windowPtr, minwidth, minheight, maxwidth,
                            maxheight);
  }
  // The minimum and maximum size of the content area of a windowed mode window
  // can be enforced with glfwSetWindowSizeLimits. The user may resize the
  // window to any size and aspect ratio within the specified limits, unless the
  // aspect ratio is also set.
  //
  // To specify only a minimum size or only a maximum one, set the value to -1
  // or GLFW_DONT_CARE
  //
  // Values in the vector defined respectively:
  // x = minwidth, y = minheight, z = maxwidth, w = maxheight
  void SetWindowSizeLimits(const glm::ivec4 limits) {
    glfwSetWindowSizeLimits(windowPtr, limits.x, limits.y, limits.z, limits.w);
  }

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
  void SetWindowAspectRatio(const int width, const int height) {
    glfwSetWindowAspectRatio(windowPtr, width, height);
  }
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
  void SetWindowAspectRatio(const glm::ivec2 ratio) {
    glfwSetWindowAspectRatio(windowPtr, ratio.x, ratio.y);
  }

  // This function sets the position, in screen coordinates, of the upper-left
  // corner of the content area of the window. If the window is a full screen
  // window, this function does nothing.
  void SetWindowPosition(const int x, const int y) {
    glfwSetWindowPos(windowPtr, x, y);
  }
  // This function sets the position, in screen coordinates, of the upper-left
  // corner of the content area of the window. If the window is a full screen
  // window, this function does nothing.
  void SetWindowPosition(const glm::ivec2 pos) {
    glfwSetWindowPos(windowPtr, pos.x, pos.y);
  }

  // This function retrieves the position, in screen coordinates, of the
  // upper-left corner of the content area of the window.
  glm::ivec2 GetWindowPosition() const {
    int xpos, ypos;
    glfwGetWindowPos(windowPtr, &xpos, &ypos);
    return glm::ivec2(xpos, ypos);
  }

  // This function sets the window title, encoded as UTF-8, of the specified
  // window.
  void SetWindowTitle(const std::string title) {
    glfwSetWindowTitle(windowPtr, title.c_str());
    this->title = title;
  }

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
  // glfwSetWindowIcon(window, 0, NULL);
  void SetWindowIcon(const GLFWimage* images, int count) {
    glfwSetWindowIcon(windowPtr, count, images);
  }

  // This function returns the handle of the monitor that the specified window
  // is in full screen on.
  GLFWmonitor* GetMonitor() { return glfwGetWindowMonitor(windowPtr); }

  // This function sets the monitor that the window uses for full screen mode
  // or, if the monitor is NULL, makes it windowed mode.
  //
  // When setting a monitor, this function updates the width, height and refresh
  // rate of the desired video mode and switches to the video mode closest to
  // it. The window position is ignored when setting a monitor.
  //
  // When the monitor is NULL, the position, width and height are used to place
  // the window content area. The refresh rate is ignored when no monitor is
  // specified.
  //
  // When a window transitions from full screen to windowed mode, this function
  // restores any previous window settings such as whether it is decorated,
  // floating, resizable, has size or aspect ratio limits, etc.
  void SetMonitor(GLFWmonitor* monitor = NULL, int xpos = 0, int ypos = 0,
                  int width = 0, int height = 0, int refreshRate = 0) {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (width == height == 0) {
      width = mode->width;
      height = mode->height;
    }
    if (refreshRate == 0) {
      refreshRate = mode->refreshRate;
    }
    glfwSetWindowMonitor(windowPtr, monitor, 0, 0, width, height, refreshRate);
  }
  // This function iconifies (minimizes) the specified window if it was
  // previously restored. If the window is already iconified, this function does
  // nothing.
  void IconifyWindow() { glfwIconifyWindow(windowPtr); }
  // This function iconifies (minimizes) the specified window if it was
  // previously restored. If the window is already iconified, this function does
  // nothing.
  void MinimizeWindow() { glfwIconifyWindow(windowPtr); }

  // This function maximizes the specified window if it was previously not
  // maximized. If the window is already maximized, this function does nothing.
  void MaximizeWindow() { glfwMaximizeWindow(windowPtr); }

  // This function restores the specified window if it was previously iconified
  // (minimized) or maximized. If the window is already restored, this function
  // does nothing.
  void RestoreWindow() { glfwRestoreWindow(windowPtr); }

  // This function hides the specified window if it was previously visible. If
  // the window is already hidden or is in full screen mode, this function does
  // nothing.
  void HideWindow() { glfwHideWindow(windowPtr); }

  // This function makes the specified window visible if it was previously
  // hidden. If the window is already visible or is in full screen mode, this
  // function does nothing.
  //
  // By default, windowed mode windows are focused when shown Set the
  // GLFW_FOCUS_ON_SHOW window hint to change this behavior for all newly
  // created windows, or change the behavior for an existing window with
  // SetWindowAttrib.
  void ShowWindow() { glfwShowWindow(windowPtr); }

  // This function brings the specified window to front and sets input focus.
  // The window should already be visible and not iconified.
  //
  // By default, both windowed and full screen mode windows are focused when
  // initially created. Set the GLFW_FOCUSED to disable this behavior.
  //
  // Also by default, windowed mode windows are focused when shown with
  // glfwShowWindow. Set the GLFW_FOCUS_ON_SHOW to disable this behavior.
  void FocusWindow() { glfwFocusWindow(windowPtr); }

  // This function requests user attention to the specified window. On platforms
  // where this is not supported, attention is requested to the application as a
  // whole.
  //
  // Once the user has given attention, usually by focusing the window or
  // application, the system will end the request automatically.
  void RequestAttention() { glfwRequestWindowAttention(windowPtr); }

 private:
  GLFWwindow* init(glm::ivec2 resolution, std::string title,
                   GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL) {
    windowPtr = glfwCreateWindow(resolution.x, resolution.y, title.c_str(),
                                 monitor, share);
  }
};

#pragma warning(default : 26495)