#pragma once

#include <engine/Ticker.h>

namespace engine::client::client {
class FrameTicker : public core::Ticker {
 public:
  // Tick each frame
  explicit FrameTicker() : Ticker(0) {}
};
}  // namespace engine::client::client
