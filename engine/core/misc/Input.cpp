#pragma once
#include "Input.h"

std::map<GLFWwindow*, Input*> Input::instances_;

Input::~Input() { instances_.erase(window_ptr_); }

void Input::init() {
  Input::instances_.size();
  glfwSetKeyCallback(this->window_ptr_, StaticKeyCallback);
  glfwSetCharCallback(this->window_ptr_, StaticCharCallback);
  glfwSetMouseButtonCallback(this->window_ptr_, StaticMouseButtonCallback);
  glfwSetCursorPosCallback(this->window_ptr_, StaticCursorPosCallback);
  glfwSetCursorEnterCallback(this->window_ptr_, StaticCursorEnterCallback);
  glfwSetScrollCallback(this->window_ptr_, StaticScrollCallback);
  glfwSetDropCallback(this->window_ptr_, StaticDropCallback);
  instances_.emplace(window_ptr_, this);
}

std::string UnicodeToUTF8(unsigned int codepoint) {
  std::string out;

  if (codepoint <= 0x7f)
    out.append(1, static_cast<char>(codepoint));
  else if (codepoint <= 0x7ff) {
    out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  } else if (codepoint <= 0xffff) {
    out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  } else {
    out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  }
  return out;
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

void Input::KeyCallback(int32_t key, int32_t scancode, int32_t action,
                        int32_t mods) {
  while (!key_callbacks_.empty()) {
    if (key_callbacks_.top().unique()) {
      key_callbacks_.pop();
    } else {
      if ((*key_callbacks_.top())(key, scancode, action, mods)) {
        key_callbacks_.pop();
      }
      break;
    }
  }
  currently_pressed_keys_[scancode] = action;
}
void Input::CharCallback(uint32_t codepoint) {
  while (!char_callbacks_.empty()) {
    if (char_callbacks_.top().unique()) {
      char_callbacks_.pop();
    } else {
      if ((*char_callbacks_.top())(codepoint)) {
        char_callbacks_.pop();
      }
      break;
    }
  }
}
void Input::MouseButtonCallback(int32_t button, int32_t action, int32_t mods) {
  while (!mouse_button_callbacks_.empty()) {
    if (mouse_button_callbacks_.top().unique()) {
      mouse_button_callbacks_.pop();
    } else {
      if ((*mouse_button_callbacks_.top())(button, action, mods)) {
        mouse_button_callbacks_.pop();
      }
      break;
    }
  }
}
void Input::CursorPosCallback(double xpos, double ypos) {
  this->xpos_ = xpos;
  this->ypos_ = ypos;
  
  while (!cursor_pos_callbacks_.empty()) {
    if (cursor_pos_callbacks_.top().unique()) {
      cursor_pos_callbacks_.pop();
    } else {
      if ((*cursor_pos_callbacks_.top())(xpos, ypos)) {
        cursor_pos_callbacks_.pop();
      }
      break;
    }
  }
}
void Input::CursorEnterCallback(int32_t entered) {
  while (!cursor_enter_callbacks_.empty()) {
    if (cursor_enter_callbacks_.top().unique()) {
      cursor_enter_callbacks_.pop();
    } else {
      if ((*cursor_enter_callbacks_.top())(entered)) {
        cursor_enter_callbacks_.pop();
      }
      break;
    }
  }
}
void Input::ScrollCallback(double xoffset, double yoffset) {
  this->xoffset_ = xoffset;
  this->yoffset_ = yoffset;
  while (!scroll_callbacks_.empty()) {
    if (scroll_callbacks_.top().unique()) {
      scroll_callbacks_.pop();
    } else {
      if ((*scroll_callbacks_.top())(xoffset, yoffset)) {
        scroll_callbacks_.pop();
      }
      break;
    }
  }
}
void Input::DropCallback(int32_t path_count, const char* paths[]) {
  std::cout << "DropEvent: " << std::endl;
  for (int i = 0; i < path_count; i++) {
    std::cout << paths[i] << std::endl;
  }

  while (!drop_callbacks_.empty()) {
    if (drop_callbacks_.top().unique()) {
      drop_callbacks_.pop();
    } else {
      if ((*drop_callbacks_.top())(path_count, paths)) {
        drop_callbacks_.pop();
      }
      break;
    }
  }
}
