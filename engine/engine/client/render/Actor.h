
#include <engine/server/Object.h>

namespace engine::client::render {

// Abstract class which defines client rendering part of an object.
class Actor {
  
  explicit virtual Actor(engine::core::Object*) = default;

}

}  // namespace engine::client::render