#pragma once

#include <core/Ticker.h>

namespace engine::client::client {
class ActorTicker : public core::Ticker {
 public:
  explicit ActorTicker(const uint32_t tickrate) : Ticker(tickrate) {}
};
}  // namespace engine::client::client
