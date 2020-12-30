#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <stack>
#include <thread>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <core/Ticker.h>

namespace engine::core {
// Functions from this class should be called only in the main thread.
class Input : public Ticker {
 public:
  using KeyCallbackFun = std::function<bool(int32_t, int32_t, int32_t, int32_t)>;
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


  ~Input();

  // This function returns the value of an input option for the
  // window. The mode must be one of GLFW_CURSOR, GLFW_STICKY_KEYS,
  // GLFW_STICKY_MOUSE_BUTTONS, GLFW_LOCK_KEY_MODS or GLFW_RAW_MOUSE_MOTION.
  int32_t input_mode(int32_t mode) {
    return glfwGetInputMode(window_ptr_, mode);
  }
  // This function sets an input mode option for the window. The mode
  // must be one of GLFW_CURSOR, GLFW_STICKY_KEYS, GLFW_STICKY_MOUSE_BUTTONS,
  // GLFW_LOCK_KEY_MODS or GLFW_RAW_MOUSE_MOTION.
  void SetInputMode(int32_t mode, int32_t value) {
    glfwSetInputMode(window_ptr_, mode, value);
  }

  int32_t GetKey(int32_t key) { return glfwGetKey(window_ptr_, key); }

  int32_t GetScancode(int32_t scancode) { 
    auto it = currently_pressed_keys_.find(scancode);
    if (it == currently_pressed_keys_.end()) {
      return 0;
    }
    return it->second;
  }

  int GetMouseButton(int button) {
    return glfwGetMouseButton(window_ptr_, button);
  }

  glm::vec2 cursor_pos() const { return glm::vec2(xpos_, ypos_); }

  void SetCursorPos(glm::vec2 pos) {
    glfwSetCursorPos(window_ptr_, pos.x, pos.y);
  }

  void SetCursor(GLFWcursor* cursor) { glfwSetCursor(window_ptr_, cursor); }

  // The function you provide is the key callback, which is called when a key is
  // pressed, repeated or released.
  //
  // You are pushing this callback onto the LIFO stack.
  // This means that this function will be called as soon as the event occurs.
  // Also, if this ptr is unique, it will be deleted.
  // If you want to delete your callback you need to make the pointer unique or
  // return true in the callback itself.
  //
  // Other callbacks in stack won't be called.
  void PushKeyCallback(const std::shared_ptr<KeyCallbackFun> ptr) {
    key_callbacks_.push(ptr);
  }

  // The function you provide is the  character callback, which is called when
  // a Unicode character is input.
  //
  // You are pushing this callback onto the LIFO stack.
  // This means that this function will be called as soon as the event occurs.
  // Also, if this ptr is unique, it will be deleted.
  // If you want to delete your callback you need to make the pointer unique or
  // return true in the callback itself.
  //
  // Other callbacks in stack won't be called.
  void PushCharCallback(const std::shared_ptr<CharCallbackFun> ptr) {
    char_callbacks_.push(ptr);
  }

  // The function you provide is the mouse button callback, which is called
  // when a mouse button is pressed or released.
  //
  // You are pushing this callback onto the LIFO stack.
  // This means that this function will be called as soon as the event occurs.
  // Also, if this ptr is unique, it will be deleted.
  // If you want to delete your callback you need to make the pointer unique or
  // return true in the callback itself.
  //
  // Other callbacks in stack won't be called.
  void PushMouseButtonCallback(
      const std::shared_ptr<MouseButtonCallbackFun> ptr) {
    mouse_button_callbacks_.push(ptr);
  }

  // The function you provide is the cursor position callback, which is called
  // when the cursor is moved. The callback is provided with the position, in
  // screen coordinates, relative to the upper-left corner of the content area
  // of the window.
  //
  // You are pushing this callback onto the LIFO stack.
  // This means that this function will be called as soon as the event occurs.
  // Also, if this ptr is unique, it will be deleted.
  // If you want to delete your callback you need to make the pointer unique or
  // return true in the callback itself.
  //
  // Other callbacks in stack won't be called.
  void PushCursorPosCallback(const std::shared_ptr<CursorPosCallbackFun> ptr) {
    cursor_pos_callbacks_.push(ptr);
  }

  /// <summary>
  /// The function you provide is the cursor boundary crossing callback, which
  /// is called when the cursor enters or leaves the content area of the window.
  ///
  /// You are pushing this callback onto the LIFO stack.
  /// This means that this function will be called as soon as the event occurs.
  /// Also, if this ptr is unique, it will be deleted.
  /// If you want to delete your callback you need to make the pointer unique or
  /// return true in the callback itself.
  ///
  /// Other callbacks in stack won't be called.
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
  ///
  /// You are pushing this callback onto the LIFO stack.
  /// This means that this function will be called as soon as the event occurs.
  /// Also, if this ptr is unique, it will be deleted.
  /// If you want to delete your callback you need to make the pointer unique or
  /// return true in the callback itself.
  ///
  // Other callbacks in stack won't be called.
  /// </summary>
  /// <param name="ptr">scroll function pointer</param>
  void PushScrollCallback(const std::shared_ptr<ScrollCallbackFun> ptr) {
    scroll_callbacks_.push(ptr);
  }

  /// <summary>
  /// The function you provide is the path drop callback, which is called when
  /// one or more dragged paths are dropped on the window.
  ///
  /// You are pushing this callback onto the LIFO stack.
  /// This means that this function will be called as soon as the event occurs.
  /// Also, if this ptr is unique, it will be deleted.
  /// If you want to delete your callback you need to make the pointer unique or
  /// return true in the callback itself.
  ///
  /// Other callbacks in stack won't be called.
  /// </summary>
  /// <param name="ptr">drop function pointer</param>
  void PushDropCallback(const std::shared_ptr<DropCallbackFun> ptr) {
    drop_callbacks_.push(ptr);
  }

  void Update(const unsigned int, const float) override;

  bool AddKeyCallback(int32_t scancode, std::shared_ptr<KeyBindCallback> kbc,
                      bool rewrite = false);

 protected:
  Input() : Ticker(1, std::this_thread::get_id()) {}

  void init();

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

  GLFWwindow* window_ptr_ = nullptr;

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
};
}  // namespace engine::core