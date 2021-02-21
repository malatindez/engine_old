
#include <memory>

#include "content/code/Render/FractalRenderer.h"
#include "engine/Object.h"

namespace content::objects {
class Fractal : public engine::core::Object {
 public:
  Fractal() : Object(1) {
    renderer_ = std::make_shared<content::render::FractalRenderer>();
  }

  [[nodiscard]] std::shared_ptr<engine::client::render::Renderer> renderer()
      override {
    return renderer_;
  }

  void Update(const uint64_t tick) override {
    // Intentionally unimplemented
  }

 private:
  std::shared_ptr<engine::client::render::Renderer> renderer_;
};
}  // namespace content::objects