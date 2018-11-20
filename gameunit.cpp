#include "gameunit.h"
#include "utils.h"
#include <math.h>
#include <iostream>

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

	//std::cout << "Max Speed: " << this->maxSpeed << "\n";
}

GameUnit::GameUnit(std::string name, int x, int y, int width, int height) : name(name), positionX(x), positionY(y), width(width), height(height) {

}

int GameUnit::getMaxHealth() const {
	return floor(this->baseHealth + this->getStamina() * this->staminaToHealthMultiplier);
}

int GameUnit::getStamina() const {
	int stamina_multiplier = 1 + (this->level - 1) * this->staminaPerLevelMultiplier;
	return this->baseStamina * stamina_multiplier;
}

bool GameUnit::setTarget(GameUnit *target) {
	this->target = target;
	return true; // will return false on wrong target
}

GameUnit* GameUnit::getTarget() const {
	return this->target;
}

void GameUnit::updatePosition(float x, float y, FacingDirection facing) {
	boost::upgrade_lock<boost::shared_mutex> lock(this->positionMutex);

	if (fabsf(this->positionX - x) > this->maxSpeed) {
		return;
	}


	if (fabsf(this->positionY - y) > this->maxSpeed) {
		return;
	}

	boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock(lock);
	this->positionX = x;
	this->positionY = y;
	this->facing = (FacingDirection)facing;
	//std::cout << "Position: " << this->positionX << ", " << this->positionY << "\n";
}

void GameUnit::moveToSpawn(float x, float y, FacingDirection facing) {
	boost::lock_guard<boost::shared_mutex> lock(this->positionMutex);

	this->positionX = x;
	this->positionY = y;
	this->facing = (FacingDirection)facing;
}

void GameUnit::update(double elapsed_time) {

}