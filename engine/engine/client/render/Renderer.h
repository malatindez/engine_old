#pragma once
#include <memory>

#include "Shader.h"
#include "engine/Object.h"

namespace engine::core {
class Object;
}
namespace engine::client::render {
class Renderer {
 public:
  virtual ~Renderer() = default;

  // Should return a shader program which will be applied before Draw() call
  // Multiple objects can have the same shader, so the RenderCore will
  // optimize shader calls
  virtual std::weak_ptr<Shader> shader() const noexcept { return {}; }

  virtual void Draw(std::weak_ptr<engine::core::Object> object) {
    // intentionally unimplemented
  }
};
}  // namespace engine::client::render

