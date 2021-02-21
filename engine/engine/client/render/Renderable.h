
#include "Shader.h"

#include <memory>

namespace engine::client::render {
class Renderable {
 public:
  virtual ~Renderable() = default;

  // Should return a shader program which will be applied before Draw() call
  // Multiple objects can have the same shader, so the Render engine will
  // optimize shader calls
  virtual std::weak_ptr<Shader> shader() const noexcept { return {}; }

  virtual void Draw() {
    // intentionally unimplemented
  }
};
}  // namespace engine::client::render