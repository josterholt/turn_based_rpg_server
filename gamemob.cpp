#include "gamemob.h"
#include <math.h>

void GameMob::loadScript(std::vector<EventNode> &nodes) {
	this->eventNodes = nodes;
}

/**
 * Using scripted directions and timing to modify state
 */
void GameMob::processScriptedState(double elapsed_time) {
	boost::upgrade_lock<boost::shared_mutex> lock(this->positionMutex);
	boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock(lock);
	
	if (this->eventNodes.size() == 0) {
		return;
	}

	EventNode node = this->eventNodes[this->eventNodeIndex];

	// Determine if character should move onto next action
	bool next_action = false;
	int tileX = 32;
	int tileY = 32;

	//std::cout << this->positionX - (this->eventOriginalX + node.velocityX * tileX) << "\n";
	//std::cout << this->positionY - (this->eventOriginalY + node.velocityY * tileY) << "\n";
	//std::cout << "DIFF: " << fabsf(this->positionX - (this->eventOriginalX + node.velocityX * tileX)) << ", " << fabsf(this->positionY - (this->eventOriginalY + node.velocityY * tileY)) << "\n";

	// @todo calculate distance in between points and interpolate
	if (fabsf(this->positionX - (this->eventOriginalX + node.velocityX * tileX)) < 10
		&& fabsf(this->positionY - (this->eventOriginalY + node.velocityY * tileY)) < 10) {
		next_action = true;
	}

	if (next_action) {
		this->velocityX = 0;
		this->velocityY = 0;

		this->eventOriginalX = this->positionX;
		this->eventOriginalY = this->positionY;
		this->eventNodeIndex++;
		if (this->eventNodeIndex >= this->eventNodes.size()) {
			this->eventNodeIndex = 0;
		}
		node = this->eventNodes[this->eventNodeIndex];
	}
	// @todo Look into improving performance of velocity, maybe caching
	
	// Calculate velocity and run some sanity checks
	this->velocityX = (node.velocityX / 60) * elapsed_time;
	this->velocityY = (node.velocityY / 60) * elapsed_time;

	/*
	if (fabsf(this->velocityX) > this->maxSpeed) {
		this->velocityX = 0;
	}

	if (fabsf(this->velocityY) > this->maxSpeed) {
		this->velocityY = 0;
	}
	*/
	
	// Detect what direction character is facing based off of velocity
	if (floorf(this->velocityX) > 0) {
		this->facing = FacingDirection::RIGHT;
	}
	else if (floorf(this->velocityX) < 0) {
		this->facing = FacingDirection::LEFT;
	}
	else if (floorf(this->velocityY) < 0) {
		this->facing = FacingDirection::UP;
	}
	else if (floorf(this->velocityY) > 0) {
		this->facing = FacingDirection::DOWN;
	}
}

void GameMob::update(double elapsed_time) {
	this->processScriptedState(elapsed_time);

	//std::cout << "Mob: " << this->positionX << ", " << this->positionY << "\n";

	this->positionX += this->velocityX;
	this->positionY += this->velocityY;
}

void GameMob::setPosition(float x, float y) {
	this->positionX = x;
	this->eventOriginalX = x;

	this->positionY = y;
	this->eventOriginalY = y;
}