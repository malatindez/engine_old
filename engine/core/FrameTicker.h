#ifndef FRAME_TICKER_H
#define FRAME_TICKER_H

#include "Ticker.h"
namespace engine::core {
class FrameTicker : public Ticker {
 public:
  explicit FrameTicker(const unsigned int tickrate) : Ticker(tickrate) {}
};
}  // namespace engine::core
#endif