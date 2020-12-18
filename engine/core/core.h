#ifndef CORE_H
#define CORE_H
#include "Ticker.h"
class GameCore {
public:
	unsigned int current_tick = 0;
	std::vector<std::shared_ptr<Ticker>> tickingObjects;
	GameCore() {
		
	}

	unsigned int getCurrentGlobalTick() const noexcept {
		return current_tick;
	}
	
	bool appendTickingObject(std::shared_ptr<Ticker> object) {
		//this->models.push_back(model);
	}

};

#endif