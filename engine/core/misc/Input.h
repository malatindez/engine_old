#pragma once
#include <GLFW/glfw3.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <thread>

#include "../FrameTicker.h"

class KeySequence {
public:
    KeySequence(size_t size, uint16_t* seq);
    KeySequence(uint16_t first);
    KeySequence(uint16_t first, uint16_t second);
    // copy constructor
    KeySequence(const KeySequence& other) noexcept
        : KeySequence(other.size_, other.sequence_) {}

    // move constructor
    KeySequence(KeySequence&& other) noexcept;

    // copy assignment
    KeySequence& operator=(const KeySequence& other) noexcept;

    // move assignment
    KeySequence& operator=(KeySequence&& other) noexcept;

    ~KeySequence();

    bool operator==(const KeySequence& ks);

    friend bool operator<(const KeySequence& lhs, const KeySequence& rhs) {
        if (rhs.size_ == lhs.size_ == 2) {
            return (*(uint32_t*)(rhs.sequence_)) < (*(uint32_t*)(lhs.sequence_));
        }
        if (rhs.size_ > lhs.size_) {
            return true;
        }
        return lhs.sequence_[0] < rhs.sequence_[1];
    }

    const uint16_t const* GetSequence() const noexcept { return sequence_; }
    size_t GetSize() const noexcept { return size_; }

protected:
    uint16_t* sequence_ = nullptr;
    size_t size_ = 0;
};

// Functions from this class should be called only in the main thread.
class Input : public FrameTicker {
public:
    static bool ProcessKeySeq(GLFWwindow* win, KeySequence seq);

    ~Input();

    std::pair<float, float> GetMousePosition() const noexcept {
        return std::pair<float, float>(xpos_, ypos_);
    }

    void AddCheckingKey(uint16_t key) noexcept { checking_.insert(key); }
    bool CheckKey(uint16_t key) const noexcept { return keys_[key]; }
    void AddCheckingSequence(KeySequence seq);
    bool CheckSequence(KeySequence seq) const;

    virtual void Update(const unsigned int, const float);

    bool AddCallback(KeySequence seq, std::function<void(int)> function,
        bool rewrite = false);

protected:
    Input() : FrameTicker(1) {}

    void init();

    // This function is the key callback, which is called when a key is pressed,
    // repeated or released.
    void KeyCallback(int key, int scancode, int action, int mods);

    // This function is the character callback, which is called when a Unicode
    // character is input.
    void CharCallback(unsigned int codepoint);

    // This function is the mouse button callback, which is called when a mouse
    // button is pressed or released.
    void MouseButtonCallback(int button, int action, int mods);

    // This function is the cursor position callback, which is called when the
    // cursor is moved. The callback is provided with the position, in screen
    // coordinates, relative to the upper-left corner of the content area of the
    // window.
    void CursorPosCallback(double xpos, double ypos);

    // This function is the cursor boundary crossing callback, which is called
    // when the cursor enters or leaves the content area of the window.
    void CursorEnterCallback(int entered);

    // This function is the scroll callback, which is called when a scrolling
    // device is used, such as a mouse wheel or scrolling area of a touchpad.
    void ScrollCallback(double xoffset, double yoffset);

    // This function is the path drop callback, which is called when one or more
    // dragged paths are dropped on the window.
    void DropCallback(int path_count, const char* paths[]);

    // Here we should store all instances of Input class. Every. One.
    // Also this static variable is used to correctly handle GLFW callbacks
    static std::map<GLFWwindow*, Input*> instances_;

    GLFWwindow* window_ptr_ = nullptr;

private:

    static void StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void StaticCharCallback(GLFWwindow* window, unsigned int codepoint);
    static void StaticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void StaticCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void StaticCursorEnterCallback(GLFWwindow* window, int entered);
    static void StaticScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void StaticDropCallback(GLFWwindow* window, int path_count, const char* paths[]);

    std::map<KeySequence, std::function<void(int)>> callbacks_;

    double xpos_ = 0, ypos_ = 0;

    bool* keys_ = new bool[65536];

    std::set<uint16_t> checking_;
};