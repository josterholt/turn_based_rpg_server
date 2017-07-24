#include "gameunit.h"
#include "utils.h"
#include <math.h>


UnitState GameUnit::getFullState() {
	boost::shared_lock<boost::shared_mutex> lock(this->positionMutex);

	UnitState state;
	state.positionX = this->positionX;
	state.positionY = this->positionY;
	state.velocityX = this->velocityX;
	state.velocityY = this->velocityY;
	state.facing = this->facing;
	return state;
}

GameUnit::GameUnit() {
	char s[25];
	gen_random(s, 24);
	s[24] = '\0';
	strcpy(this->token, s);

	this->positionX = 100;
	this->positionY = 2000;

	std::cout << "Max Speed: " << this->maxSpeed << "\n";
}

void GameUnit::updatePosition(float x, float y, FacingDirection facing) {
	boost::upgrade_lock<boost::shared_mutex> lock(this->positionMutex);
	boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock(lock);

	if (fabsf(this->positionX - x) > this->maxSpeed) {
		return;
	}


	if (fabsf(this->positionY - y) > this->maxSpeed) {
		return;
	}

	this->positionX = x;
	this->positionY = y;
	this->facing = (FacingDirection)facing;
	std::cout << "Position: " << this->positionX << ", " << this->positionY << "\n";
}

void GameUnit::moveToSpawn(float x, float y, FacingDirection facing) {
	boost::upgrade_lock<boost::shared_mutex> lock(this->positionMutex);
	boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock(lock);

	this->positionX = x;
	this->positionY = y;
	this->facing = (FacingDirection)facing;
}

void GameUnit::update(double elapsed_time) {

}