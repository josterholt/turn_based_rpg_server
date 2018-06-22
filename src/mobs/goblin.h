#pragma once
#ifndef _GOBLIN_H_
#define _GOBLIN_H_

#include "../../gameunit.h"
#include "string"

class Goblin : public GameUnit {
	int bufferedY = 0;

public:
	Goblin(std::string name, int x, int y, int width, int height);
	std::pair<int, int> getFacingVelocity();
};
#endif