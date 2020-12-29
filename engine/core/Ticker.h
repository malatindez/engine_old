#pragma once
#pragma warning(disable : 4100)
namespace engine::core {
class Ticker {
 public:
  unsigned int tickrate_;
  explicit Ticker(unsigned int tickrate) : tickrate_(tickrate) { }
  unsigned int getTickRate() const { return tickrate_; }
};
}
#pragma warning(default : 4100)