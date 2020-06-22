#ifndef TICKER_H
#define TICKER_H

template<int tickrate>
class Ticker {
public:
	unsigned int getTickRate() const {
		return tickrate; 
	}
	// Parameter of this function is a current global tick(from a game start)
	void Update(const unsigned int tick) noexcept {
		
	}
};


#endif