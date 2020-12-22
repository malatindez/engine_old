#pragma once
#include <functional>
#include <algorithm>
#include <thread>
#include <mutex>
#include <set>
#include <map>
#include <GLFW/glfw3.h>

#include "../FrameTicker.h"
#include "../../render/misc/Window.h"

struct KeySequence {
  uint16_t* sequence = nullptr;
  size_t size = 0;
  KeySequence(size_t size, uint16_t* seq) {
    if (size > 5 or size == 0) {
      return;
    }
    sequence = new uint16_t[size];
    memcpy(sequence, seq, sizeof(uint16_t) * size);
    this->size = size;
  }
  KeySequence(uint16_t first) {
    size = 1;
    sequence = new uint16_t[1];
    sequence[0] = first;
  }
  KeySequence(uint16_t first, uint16_t second) {
    size = 2;
    sequence = new uint16_t[2];
    sequence[0] = first;
    sequence[1] = second;
  }
  KeySequence(uint16_t first, uint16_t second, uint16_t third) {
    size = 3;
    sequence = new uint16_t[3];
    sequence[0] = first;
    sequence[1] = second;
    sequence[2] = third;
  }
  KeySequence(uint16_t first, uint16_t second, uint16_t third,
              uint16_t fourth) {
    size = 4;
    sequence = new uint16_t[4];
    sequence[0] = first;
    sequence[1] = second;
    sequence[2] = third;
    sequence[3] = fourth;
  }
  KeySequence(uint16_t first, uint16_t second, uint16_t third, uint16_t fourth,
              uint16_t fifth) {
    size = 5;
    sequence = new uint16_t[5];
    sequence[0] = first;
    sequence[1] = second;
    sequence[2] = third;
    sequence[3] = fourth;
    sequence[4] = fifth;
  }
  // copy constructor
  KeySequence(const KeySequence& other) noexcept
      : KeySequence(other.size, other.sequence) {}

  // move constructor
  KeySequence(KeySequence&& other) noexcept {
    this->size = other.size;
    this->sequence = other.sequence;
    other.sequence = nullptr;
  }

  // copy assignment
  KeySequence& operator=(const KeySequence& other) noexcept {
    sequence = new uint16_t[other.size];
    memcpy(sequence, other.sequence, sizeof(uint16_t) * other.size);
    this->size = other.size;
  }

  // move assignment
  KeySequence& operator=(KeySequence&& other) noexcept {
    this->size = other.size;
    this->sequence = other.sequence;
    other.sequence = nullptr;
  }

  ~KeySequence() {
    if (sequence != nullptr) {
      delete[] sequence;
    }
    sequence = nullptr;
  }

  bool operator==(const KeySequence& ks) {
    if (ks.size != size) {
      return false;
    }
    for (size_t i = 0; i < size; i++) {
      if (ks.sequence[i] != sequence[i]) {
        return false;
      }
    }
    return true;
  }
};

class Input : public FrameTicker {
 public:
  static bool processKeySeq(GLFWwindow* win, KeySequence seq) {
    bool flag = true;
    for (size_t i = 0; i < seq.size; i++) {
      if (glfwGetKey(win, seq.sequence[i]) != GLFW_PRESS) {
        flag = false;
        break;
      }
    }
    return flag;
  }
  
  ~Input() { }

  std::pair<float, float> GetMousePosition() const noexcept {
    return std::pair<float, float>(xpos_, ypos_);
  }

  void AddCheckingKey(uint16_t key) noexcept { checking_.insert(key); }
  void AddCheckingSequence(KeySequence seq) noexcept {
    for (size_t i = 0; i < seq.size; i++) {
      checking_.insert(seq.sequence[i]);
    }
  }
  bool CheckSequence(KeySequence seq) const noexcept {
    for (size_t i = 0; i < seq.size; i++) {
      if (not keys_[seq.sequence[i]]) {
        return false;
      }
    }
    return true;
  }
  bool CheckKey(uint16_t key) const noexcept { return keys_[key]; }

  virtual void Update(const unsigned int, const float) noexcept {
    for (std::set<uint16_t>::iterator i = checking_.begin(); i != checking_.end();
         i++) {
      keys_[(*i)] = (glfwGetKey(window_ptr_, (*i)) == GLFW_PRESS);
    }
    glfwGetCursorPos(window_ptr_, &xpos_, &ypos_);
  }

  bool AddCallback(KeySequence seq, std::function<void(int)> function, bool rewrite = false) {
    AddCheckingSequence(seq);
    if (callbacks_.find(seq) != callbacks_.end() and not rewrite) {
      return false;
    }
    callbacks_.emplace(seq, function);
    return true;
  }

 protected:
  Input() : FrameTicker(1) { }

  void init(GLFWwindow* win) { this->window_ptr_ = win; }

  GLFWwindow* window_ptr_ = nullptr;

 private:
  std::map<KeySequence, std::function<void(int)>> callbacks_;
  double xpos_ = 0, ypos_ = 0;
  bool* keys_ = new bool[65536];
  std::set<uint16_t> checking_;
};