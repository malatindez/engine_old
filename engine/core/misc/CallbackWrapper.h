#pragma once
#include <GLFW/glfw3.h>

#include <functional>
#include <memory>
#include <map>

#include "Window.h"
#include <iostream>
namespace engine::core::misc {

class NONE_TYPE {};

template <class _Fty, class _Fty2, typename _Param1 = NONE_TYPE,
          typename _Param2 = NONE_TYPE, typename _Param3 = NONE_TYPE,
          typename _Param4 = NONE_TYPE>
void WrapFunction(std::shared_ptr<std::function<_Fty>> class_func,
                   std::function<_Fty2(_Fty2)> setter) {
  if (std::is_same<_Param1, NONE_TYPE>::value) {
    auto func = [](GLFWwindow* window) { } setter(func);
  } else if (std::is_same<_Param2, NONE_TYPE>::value) {
    auto func = [](GLFWwindow* window, _Param1 p1) {
      std::cout << p1;
    } setter(func);
  } else if (std::is_same<_Param3, NONE_TYPE>::value) {
    auto func = [](GLFWwindow* window, _Param1 p1, _Param2 p2) {
      std::cout << p1 << " " << p2;
    } setter(func);
  } else if (std::is_same<_Param4, NONE_TYPE>::value) {
    auto func = [](GLFWwindow* window, _Param1 p1, _Param2 p2, _Param3 p3) {
      std::cout << p1 << " " << p2 <<  " " << p3;
    } setter(func);
  } else {
    auto func = [](GLFWwindow* window, _Param1 p1, _Param2 p2, _Param3 p3,
                   _Param4 p4) {
      std::cout << p1 << " " << p2 << " " << p3 << " " << p4;
    } setter(func);
  }
}
}  // namespace engine::core::misc