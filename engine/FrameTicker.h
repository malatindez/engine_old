#ifndef FRAME_TICKER_H
#define FRAME_TICKER_H

#include "Ticker.h"
template<int tick=1>
class FrameTicker : public Ticker<tick> {

};

#endif