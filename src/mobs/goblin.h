#pragma once
#ifndef _GOBLIN_H_
#define _GOBLIN_H_

#include "../../gameunit.h"
#include "string"

class Goblin : public GameUnit {
	int bufferedY;

public:
	Goblin(std::string name, int x, int y, int width, int height);
	int getFacingVelocity;
};
#endif