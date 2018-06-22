#include "goblin.h"

Goblin::Goblin(std::string name, int x, int y, int width, int height) : GameUnit(name, x, y, width, height) {
	this->type = "GOBLIN";
	this->experienceGiven = 50;
	this->baseHealth = 5;
	this->health = this->getMaxHealth();
	this->image = "/assets/profiles/goblin.png";
	this->bufferedY = this->positionY;
}

std::pair<int, int> Goblin::getFacingVelocity() {
	int x = 0;
	int y = 0;

	if (this->facing == FacingDirection::RIGHT) {
		x = this->height;
		y = 0;
	} else if(this->facing == FacingDirection::DOWN) {
		x = 0;
		y = this->height;
	}
	else if (this->facing == FacingDirection::LEFT) {
		x = -this->width;
		y = 0;
	}
	else if (this->facing == FacingDirection::UP) {
		x = 0;
		y = -this->width;
	}

	return std::make_pair(x, y);
}