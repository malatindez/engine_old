#ifndef TICKER_H
#define TICKER_H
#pragma warning(disable : 4100)
class Ticker {
 public:
  unsigned int tickrate_;
  Ticker(unsigned int tickrate) { this->tickrate_ = tickrate; }
  unsigned int getTickRate() const { return tickrate_; }
  // Parameter of this function is a current global tick(from a game start)
  void Update(const unsigned int tick, const float delta) noexcept {}
};
#pragma warning(default : 4100)
#endif