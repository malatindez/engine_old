#ifndef FRAME_TICKER_H
#define FRAME_TICKER_H

#include "Ticker.h"
class FrameTicker : public Ticker{
public:
	FrameTicker(const unsigned int tickrate ) : Ticker(tickrate) {}
};

#endif