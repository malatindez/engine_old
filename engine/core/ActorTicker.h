#pragma once

#include "Ticker.h"

namespace engine::core {
class ActorTicker : public Ticker {
 public:
  explicit ActorTicker(const uint32_t tickrate) : Ticker(tickrate) {}
};
}  // namespace engine::core
