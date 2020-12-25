#pragma once
#include "Input.h"

KeySequence::KeySequence(size_t size, uint16_t* seq) {
  if (size > 2 or size == 0) {
    return;
  }
  sequence_ = new uint16_t[size];
  memcpy(sequence_, seq, sizeof(uint16_t) * size);
  this->size_ = size;
}

KeySequence::KeySequence(uint16_t first) {
  size_ = 1;
  sequence_ = new uint16_t[1];
  sequence_[0] = first;
}

KeySequence::KeySequence(uint16_t first, uint16_t second) {
  size_ = 2;
  sequence_ = new uint16_t[2];
  sequence_[0] = first;
  sequence_[1] = second;
}

KeySequence::KeySequence(KeySequence&& other) noexcept {
  this->size_ = other.size_;
  this->sequence_ = other.sequence_;
  other.sequence_ = nullptr;
}

KeySequence& KeySequence::operator=(const KeySequence& other) noexcept {
  sequence_ = new uint16_t[other.size_];
  memcpy(sequence_, other.sequence_, sizeof(uint16_t) * other.size_);
  this->size_ = other.size_;
  return (*this);
}

KeySequence& KeySequence::operator=(KeySequence&& other) noexcept {
  this->size_ = other.size_;
  this->sequence_ = other.sequence_;
  other.sequence_ = nullptr;
  return (*this);
}

KeySequence::~KeySequence() {
  if (sequence_ != nullptr) {
    delete[] sequence_;
  }
  sequence_ = nullptr;
}

bool KeySequence::operator==(const KeySequence& ks) {
  if (ks.size_ != size_) {
    return false;
  }
  for (size_t i = 0; i < size_; i++) {
    if (ks.sequence_[i] != sequence_[i]) {
      return false;
    }
  }
  return true;
}

bool Input::ProcessKeySeq(GLFWwindow* win, KeySequence seq) {
  bool flag = true;
  for (size_t i = 0; i < seq.GetSize(); i++) {
    if (glfwGetKey(win, seq.GetSequence()[i]) != GLFW_PRESS) {
      flag = false;
      break;
    }
  }
  return flag;
}

void Input::AddCheckingSequence(KeySequence seq) {
  for (size_t i = 0; i < seq.GetSize(); i++) {
    checking_.insert(seq.GetSequence()[i]);
  }
}

bool Input::CheckSequence(KeySequence seq) const {
  for (size_t i = 0; i < seq.GetSize(); i++) {
    if (not keys_[seq.GetSequence()[i]]) {
      return false;
    }
  }
  return true;
}

void Input::Update(const unsigned int, const float) {
  for (std::set<uint16_t>::iterator i = checking_.begin(); i != checking_.end();
       i++) {
    keys_[(*i)] = (glfwGetKey(window_ptr_, (*i)) == GLFW_PRESS);
  }
  glfwGetCursorPos(window_ptr_, &xpos_, &ypos_);
}

bool Input::AddCallback(KeySequence seq, std::function<void(int)> function,
                        bool rewrite) {
  AddCheckingSequence(seq);
  if (callbacks_.find(seq) != callbacks_.end() and not rewrite) {
    return false;
  }
  callbacks_.emplace(seq, function);
  return true;
}

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
void Input::KeyCallback(int key, int scancode, int action, int mods) {
  std::cout << "KeyCallback: " << key << " " << scancode << " " << action << " " << mods
            << std::endl;
}

void Input::StaticCharCallback(GLFWwindow* window, unsigned int codepoint) {
  instances_.find(window)->second->CharCallback(codepoint);
}
void Input::CharCallback(unsigned int codepoint) {
  std::cout << "CharEvent: " << UnicodeToUTF8(codepoint) << std::endl;
}

void Input::StaticMouseButtonCallback(GLFWwindow* window, int button,
                                      int action, int mods) {
  instances_.find(window)->second->MouseButtonCallback(button, action, mods);
}
void Input::MouseButtonCallback(int button, int action, int mods) {
  std::cout << "MouseButtonEvent: " << button << " " << action << " " << mods
            << std::endl;
}

void Input::StaticCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  instances_.find(window)->second->CursorPosCallback(xpos, ypos);
}
void Input::CursorPosCallback(double xpos, double ypos) {
  //std::cout << "CursorPosEvent: " << xpos << " " << ypos << std::endl;
}

void Input::StaticCursorEnterCallback(GLFWwindow* window, int entered) {
  instances_.find(window)->second->CursorEnterCallback(entered);
}
void Input::CursorEnterCallback(int entered) {
  std::cout << "CursorEnterEvent: " << entered << std::endl;
}


void Input::StaticScrollCallback(GLFWwindow* window, double xoffset,
                                 double yoffset) {
  instances_.find(window)->second->ScrollCallback(xoffset, yoffset);
}
void Input::ScrollCallback(double xpos, double ypos) {
  std::cout << "ScrollEvent: " << xpos << " " << ypos << std::endl;
}


void Input::StaticDropCallback(GLFWwindow* window, int path_count,
                               const char* paths[]) {
  instances_.find(window)->second->DropCallback(path_count, paths);
}
void Input::DropCallback(int path_count, const char* paths[]) {
  std::cout << "DropEvent: " << std::endl;
  for (int i = 0; i < path_count; i++) {
    std::cout << paths[i] << std::endl;
  }
}
