#pragma once
#pragma warning(disable : 4100)
namespace engine::core {
class Ticker {
 public:
  unsigned int tickrate_;
  Ticker(unsigned int tickrate) { this->tickrate_ = tickrate; }
  unsigned int getTickRate() const { return tickrate_; }
  // Parameter of this function is a current global tick(from a game start)
  void Update(const unsigned int tick, const float delta) noexcept {}
};
}  // namespace engine::core
#pragma warning(default : 4100)