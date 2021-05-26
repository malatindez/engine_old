#pragma once
#include <GLFW/glfw3.h>
#include <engine/Ticker.h>

#include <algorithm>
#include <functional>
#include <glm/vec2.hpp>
#include <map>
#include <mutex>
#include <set>
#include <stack>
#include <thread>
#include <vector>

namespace engine::client {
// Functions from this class should be called only in the main thread.

/// <summary>
///
/// Quick note about callbacks:
/// You are pushing callbacks onto the LIFO stacks(each callback has its own
/// stack). This means that the callback will be called as soon as the event
/// occurs. Also, if shared_ptr is unique, it will be deleted. If you want to
/// delete your callback you need to make the pointer unique or return true in
/// the callback itself.
///
/// Other callbacks in the stack won't be called.
/// </summary>
class Input : public core::Ticker {
 public:
  using KeyCallbackFun =
      std::function<bool(int32_t, int32_t, int32_t, int32_t)>;
  using CharCallbackFun = std::function<bool(uint32_t)>;
  using MouseButtonCallbackFun = std::function<bool(int32_t, int32_t, int32_t)>;
  using CursorPosCallbackFun = std::function<bool(double, double)>;
  using CursorEnterCallbackFun = std::function<bool(int32_t)>;
  using ScrollCallbackFun = std::function<bool(double, double)>;
  using DropCallbackFun = std::function<bool(int32_t, const char*[])>;

  using KeyBindCallback = std::function<bool(int32_t, int32_t)>;

  /* Disable copy and move semantics. */
  Input(const Input&) = delete;
  Input(Input&&) = delete;
  Input& operator=(const Input&) = delete;
  Input& operator=(Input&&) = delete;

  ~Input() override;

  /// <summary>
  /// This function returns the value of an input option for the specified
  /// window.
  /// </summary>
  /// <param name="mode">One of GLFW_CURSOR, GLFW_STICKY_KEYS,
  /// GLFW_STICKY_MOUSE_BUTTONS, GLFW_LOCK_KEY_MODS or
  /// GLFW_RAW_MOUSE_MOTION.</param> <returns>Returns the value of an input
  /// option for the specified window.</returns>
  [[nodiscard]] int32_t input_mode(int32_t mode) {
    return glfwGetInputMode(window_ptr_, mode);
  }

  /// <summary>
  /// This function sets an input mode option for the window. The mode
  /// must be one of GLFW_CURSOR, GLFW_STICKY_KEYS, GLFW_STICKY_MOUSE_BUTTONS,
  /// GLFW_LOCK_KEY_MODS or GLFW_RAW_MOUSE_MOTION.
  /// </summary>
  /// <param name="mode">One of GLFW_CURSOR, GLFW_STICKY_KEYS,
  /// GLFW_STICKY_MOUSE_BUTTONS, GLFW_LOCK_KEY_MODS or
  /// GLFW_RAW_MOUSE_MOTION.</param> <param name="value">The new value of the
  /// specified input mode.</param>
  void SetInputMode(int32_t mode, int32_t value) {
    glfwSetInputMode(window_ptr_, mode, value);
  }

  /// <summary>
  /// Returns status of the key.
  /// If the key is pressed, then returns GLFW_PRESS, i.e. 1.
  /// If the key held down, then returns GLFW_REPEAT, i.e. 2.
  /// If the key is released(not pressed nor repeated), then returns
  /// GLFW_RELEASE, i.e. 0.
  /// </summary>
  /// <param name="scancode">The scancode of the window.</param>
  /// <returns>Status of the key.</returns>
  [[nodiscard]] int32_t key_status(int32_t scancode) {
    auto it = currently_pressed_keys_.find(scancode);
    if (it == currently_pressed_keys_.end()) {
      return 0;
    }
    return it->second;
  }

  /// <summary>
  /// This function returns the last state reported for the specified mouse
  /// button to the specified window.
  /// The returned state is one of GLFW_PRESS or GLFW_RELEASE.
  /// </summary>
  /// <param name="button">The desired mouse button.</param>
  /// <returns>Last state reported for the specified mouse button.</returns>
  [[nodiscard]] int GetMouseButton(int button) {
    return glfwGetMouseButton(window_ptr_, button);
  }

  /// <summary>
  /// returns current mouse position, relative to the top-left corner of
  /// the content area.
  /// </summary>
  /// <returns>mouse position</returns>
  [[nodiscard]] glm::vec2 cursor_pos() const { return glm::vec2(xpos_, ypos_); }

  /// <summary>
  /// returns current mouse position, relative to the top-left corner of
  /// the content area.
  /// </summary>
  /// <returns>mouse position</returns>
  [[nodiscard]] glm::vec2 cursor_position() const {
    return glm::vec2(xpos_, ypos_);
  }

  /// <summary>
  /// Sets the position of mouse in the current window.
  /// </summary>
  /// <param name="pos">mouse position</param>
  void SetCursorPos(glm::vec2 pos) {
    glfwSetCursorPos(window_ptr_, pos.x, pos.y);
  }

  /// <summary>
  /// Sets the position of mouse in the current window.
  /// </summary>
  /// <param name="xpos">The new mouse x-coordinate, relative to the left
  /// edge of the content area.</param>
  /// <param name="ypos">The new mouse y-coordinate, relative to the top
  /// edge of the content area.</param>
  void SetCursorPos(double xpos, double ypos) {
    glfwSetCursorPos(window_ptr_, xpos, ypos);
  }

  /// <summary>
  /// Sets the custom cursor for the current window.
  /// If you want to remove changes you can provide nullptr as argument to this
  /// fuction.
  /// </summary>
  /// <param name="cursor">cursor</param>
  void SetCursor(GLFWcursor* cursor) { glfwSetCursor(window_ptr_, cursor); }

  /// <summary>
  /// The function you provide is the key callback, which is called when a key
  /// is pressed, repeated or released.
  /// </summary>
  /// <param name="ptr">key function pointer</param>
  void PushKeyCallback(const std::shared_ptr<KeyCallbackFun> ptr) {
    key_callbacks_.push(ptr);
  }

  /// <summary>
  /// The function you provide is the  character callback, which is called when
  /// a Unicode character is input.
  /// </summary>
  /// <param name="ptr">char function pointer</param>
  void PushCharCallback(const std::shared_ptr<CharCallbackFun> ptr) {
    char_callbacks_.push(ptr);
  }

  /// <summary>
  /// The function you provide is the mouse button callback, which is called
  /// when a mouse button is pressed or released.
  /// </summary>
  /// <param name="ptr">mouse button function pointer</param>
  void PushMouseButtonCallback(
      const std::shared_ptr<MouseButtonCallbackFun> ptr) {
    mouse_button_callbacks_.push(ptr);
  }

  /// <summary>
  /// The function you provide is the cursor position callback, which is called
  /// when the cursor is moved. The callback is provided with the position, in
  /// screen coordinates, relative to the upper-left corner of the content area
  /// of the window. Other callbacks in stack won't be called.
  /// </summary>
  /// <param name="ptr">cursor position function pointer</param>
  void PushCursorPosCallback(const std::shared_ptr<CursorPosCallbackFun> ptr) {
    cursor_pos_callbacks_.push(ptr);
  }

  /// <summary>
  /// The function you provide is the cursor boundary crossing callback, which
  /// is called when the cursor enters or leaves the content area of the window.
  /// </summary>
  /// <param name="ptr">cursor enter function pointer</param>
  void PushCursorEnterCallback(
      const std::shared_ptr<CursorEnterCallbackFun> ptr) {
    cursor_enter_callbacks_.push(ptr);
  }

  /// <summary>
  /// The function you provide is the scroll callback, which is called when a
  /// scrolling device is used, such as a mouse wheel or scrolling area of a
  /// touchpad.
  /// </summary>
  /// <param name="ptr">scroll function pointer</param>
  void PushScrollCallback(const std::shared_ptr<ScrollCallbackFun> ptr) {
    scroll_callbacks_.push(ptr);
  }

  /// <summary>
  /// The function you provide is the path drop callback, which is called when
  /// one or more dragged paths are dropped on the window.
  /// </summary>
  /// <param name="ptr">drop function pointer</param>
  void PushDropCallback(const std::shared_ptr<DropCallbackFun> ptr) {
    drop_callbacks_.push(ptr);
  }

  void Update(const uint64_t) override;

  bool AddKeyCallback(int32_t scancode, std::shared_ptr<KeyBindCallback> kbc,
                      bool rewrite = false);

 protected:
  Input() : Ticker(1U, std::this_thread::get_id()) {}

  void Init(GLFWwindow* window_ptr);

  void KeyCallback(int32_t key, int32_t scancode, int32_t action, int32_t mods);
  void CharCallback(uint32_t codepoint);
  void MouseButtonCallback(int32_t button, int32_t action, int32_t mods);
  void CursorPosCallback(double xpos, double ypos);
  void CursorEnterCallback(int32_t entered);
  void ScrollCallback(double xoffset, double yoffset);
  void DropCallback(int32_t path_count, const char* paths[]);

  // This static variable is used to correctly handle GLFW callbacks.
  // Here we should store all instances of Input class. Every. One.
  static std::map<GLFWwindow*, Input*> instances_;

 private:
  static void StaticCharCallback(GLFWwindow* window, unsigned int codepoint);

  static void StaticCursorEnterCallback(GLFWwindow* window, int entered);

  static void StaticKeyCallback(GLFWwindow* window, int key, int scancode,
                                int action, int mods);

  static void StaticMouseButtonCallback(GLFWwindow* window, int button,
                                        int action, int mods);

  static void StaticScrollCallback(GLFWwindow* window, double xoffset,
                                   double yoffset);

  static void StaticCursorPosCallback(GLFWwindow* window, double xpos,
                                      double ypos);

  static void StaticDropCallback(GLFWwindow* window, int path_count,
                                 const char* paths[]);

  double xpos_ = 0;
  double ypos_ = 0;

  double xoffset_ = 0;
  double yoffset_ = 0;

  // scancode, action(currently pressed or were released)
  std::map<int32_t, int32_t> currently_pressed_keys_;

  // scancode and function to call.
  std::map<int32_t, std::shared_ptr<KeyBindCallback>> key_bind_callbacks_;

  std::stack<std::shared_ptr<KeyCallbackFun>> key_callbacks_;
  std::stack<std::shared_ptr<CharCallbackFun>> char_callbacks_;
  std::stack<std::shared_ptr<MouseButtonCallbackFun>> mouse_button_callbacks_;
  std::stack<std::shared_ptr<CursorPosCallbackFun>> cursor_pos_callbacks_;
  std::stack<std::shared_ptr<CursorEnterCallbackFun>> cursor_enter_callbacks_;
  std::stack<std::shared_ptr<ScrollCallbackFun>> scroll_callbacks_;
  std::stack<std::shared_ptr<DropCallbackFun>> drop_callbacks_;

  GLFWwindow* window_ptr_ = nullptr;
};
}  // namespace engine::client