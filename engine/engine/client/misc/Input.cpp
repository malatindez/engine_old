#pragma once
#include "Input.h"

namespace engine::client {
std::map<GLFWwindow*, Input*> Input::instances_;

Input::~Input() { instances_.erase(window_ptr_); }

void Input::Init(GLFWwindow* window_ptr) {
  this->window_ptr_ = window_ptr;
  glfwSetKeyCallback(this->window_ptr_, StaticKeyCallback);
  glfwSetCharCallback(this->window_ptr_, StaticCharCallback);
  glfwSetMouseButtonCallback(this->window_ptr_, StaticMouseButtonCallback);
  glfwSetCursorPosCallback(this->window_ptr_, StaticCursorPosCallback);
  glfwSetCursorEnterCallback(this->window_ptr_, StaticCursorEnterCallback);
  glfwSetScrollCallback(this->window_ptr_, StaticScrollCallback);
  glfwSetDropCallback(this->window_ptr_, StaticDropCallback);
  instances_.emplace(window_ptr_, this);
}

void Input::StaticKeyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
  instances_.find(window)->second->KeyCallback(key, scancode, action, mods);
}

void Input::StaticCharCallback(GLFWwindow* window, unsigned int codepoint) {
  instances_.find(window)->second->CharCallback(codepoint);
}
void Input::StaticMouseButtonCallback(GLFWwindow* window, int button,
                                      int action, int mods) {
  instances_.find(window)->second->MouseButtonCallback(button, action, mods);
}
void Input::StaticCursorPosCallback(GLFWwindow* window, double xpos,
                                    double ypos) {
  instances_.find(window)->second->CursorPosCallback(xpos, ypos);
}
void Input::StaticCursorEnterCallback(GLFWwindow* window, int entered) {
  instances_.find(window)->second->CursorEnterCallback(entered);
}
void Input::StaticScrollCallback(GLFWwindow* window, double xoffset,
                                 double yoffset) {
  instances_.find(window)->second->ScrollCallback(xoffset, yoffset);
}
void Input::StaticDropCallback(GLFWwindow* window, int path_count,
                               const char* paths[]) {
  instances_.find(window)->second->DropCallback(path_count, paths);
}

void Input::Update(const uint64_t) {
  // after each event update we shall call each callback
  for (auto i = key_bind_callbacks_.begin(); i != key_bind_callbacks_.end();) {
    auto cpk_itr = currently_pressed_keys_.find(i->first);
    bool del = false;
    // if the key is in the map, then it is currently pressed or was released
    // just now
    if (cpk_itr != currently_pressed_keys_.end() && cpk_itr->second != -1) {
      // if callback returns true value - we shall delete this callback from
      // map
      del = (*i->second)(cpk_itr->first, cpk_itr->second);
      // if the action is KEY_RELEASE -> we shall delete this scancode from
      // currently_pressed_keys_
      if (cpk_itr->second == 0) {
        currently_pressed_keys_.erase(cpk_itr);
      }
    }

    // iterate
    if (del) {
      i = key_bind_callbacks_.erase(i);
    } else {
      i++;
    }
  }
}

bool Input::AddKeyCallback(int32_t scancode,
                           std::shared_ptr<KeyBindCallback> kbc, bool rewrite) {
  if (key_bind_callbacks_.find(scancode) == key_bind_callbacks_.end() ||
      rewrite) {
    key_bind_callbacks_[scancode] = kbc;
    rewrite = true;
  }
  return rewrite;
}

void Input::KeyCallback(int32_t key, int32_t scancode, int32_t action,
                        int32_t mods) {
  while (!key_callbacks_.empty()) {
    if (key_callbacks_.top().use_count() == 1) {
      key_callbacks_.pop();
      continue;
    }
    if ((*key_callbacks_.top())(key, scancode, action, mods)) {
      key_callbacks_.pop();
    }
    break;
  }
  currently_pressed_keys_[scancode] = action;
}
void Input::CharCallback(uint32_t codepoint) {
  while (!char_callbacks_.empty()) {
    if (char_callbacks_.top().use_count() == 1) {
      char_callbacks_.pop();
      continue;
    }
    if ((*char_callbacks_.top())(codepoint)) {
      char_callbacks_.pop();
    }
    break;
  }
}
void Input::MouseButtonCallback(int32_t button, int32_t action, int32_t mods) {
  while (!mouse_button_callbacks_.empty()) {
    if (mouse_button_callbacks_.top().use_count() == 1) {
      mouse_button_callbacks_.pop();
      continue;
    }
    if ((*mouse_button_callbacks_.top())(button, action, mods)) {
      mouse_button_callbacks_.pop();
    }
    break;
  }
}
void Input::CursorPosCallback(double xpos, double ypos) {
  this->xpos_ = xpos;
  this->ypos_ = ypos;

  while (!cursor_pos_callbacks_.empty()) {
    if (cursor_pos_callbacks_.top().use_count() == 1) {
      cursor_pos_callbacks_.pop();
      continue;
    }
    if ((*cursor_pos_callbacks_.top())(xpos, ypos)) {
      cursor_pos_callbacks_.pop();
    }
    break;
  }
}
void Input::CursorEnterCallback(int32_t entered) {
  while (!cursor_enter_callbacks_.empty()) {
    if (cursor_enter_callbacks_.top().use_count() == 1) {
      cursor_enter_callbacks_.pop();
      continue;
    }
    if ((*cursor_enter_callbacks_.top())(entered)) {
      cursor_enter_callbacks_.pop();
    }
    break;
  }
}
void Input::ScrollCallback(double xoffset, double yoffset) {
  this->xoffset_ = xoffset;
  this->yoffset_ = yoffset;
  while (!scroll_callbacks_.empty()) {
    if (scroll_callbacks_.top().use_count() == 1) {
      scroll_callbacks_.pop();
      continue;
    }
    if ((*scroll_callbacks_.top())(xoffset, yoffset)) {
      scroll_callbacks_.pop();
    }
    break;
  }
}
void Input::DropCallback(int32_t path_count, const char* paths[]) {
  while (!drop_callbacks_.empty()) {
    if (drop_callbacks_.top().use_count() == 1) {
      drop_callbacks_.pop();
      continue;
    }
    if ((*drop_callbacks_.top())(path_count, paths)) {
      drop_callbacks_.pop();
    }
    break;
  }
}

}  // namespace engine::client