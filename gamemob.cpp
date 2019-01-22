#include "gamemob.h"
#include <math.h>
#include "src/AStarPathFinding.h"
#include <iostream>

GameMob::GameMob(GameState* game_state) : _gameState(game_state) {}

void GameMob::loadScript(std::vector<EventNode> &nodes) {
	this->eventNodes = nodes;
}

void GameMob::setMap(GameMap game_map) {
	this->_map = game_map;
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
}

Point raycast(int x, int y, int d) {
	float rad = atan2(y, x);
	int new_y = sin(rad) * d;
	int new_x = cos(rad) * d;
	return Point(new_x, new_y);
}

bool GameMob::_raycastTiles(GameMap map, GameUnit* source, GameUnit* target, int distance) {
	// Is within view distance (ignoring any visual blockers)
	int x = target->positionX - source->positionX;
	int y = target->positionY - source->positionY;

	if (sqrt(x*x + y * y) > distance) {
		return false;
	}

	// Is there anything blocking line of sight?
	int start_tile = coords_to_index(map.map_width, map.tile_size, source->positionX, source->positionY);
	int target_tile = coords_to_index(map.map_width, map.tile_size, target->positionX, target->positionY);
	int view_x;
	int view_y;
	int tile_view_distance = floor(distance / map.tile_size);

	int current_distance = 0;
	while (current_distance <= tile_view_distance) {
		Point output = raycast(source->positionX - target->positionX, source->positionY - target->positionY, current_distance);
		//Point output = raycast(map.tile_size, map.tile_size, current_distance);
		int index = coords_to_index(map.map_width, map.tile_size, source->positionX + floor(output.x / map.tile_size), source->positionY + (output.y / map.tile_size));

		std::cout << "### Checking index " << index << "\n";

		// @todo Need to add right and bottom boundaries
		if (index >= 0) {
			if (map.tiles->at(index) == 1) {
				std::cout << "### Index is blocked, falling out\n";
				return false;
			}

			if (index == target_tile) {
				std::cout << "### Found target\n";
				return true;
			}

		}
		std::cout << "### Continuing to next check\n";
		current_distance += map.tile_size;
	}
}

void GameMob::update(double elapsed_time) {
	this->time_elapsed += elapsed_time;
	if (this->time_elapsed > 1000.0f) {
		this->time_elapsed = 0;
		if (this->health > 0) {
			/*
			this->processScriptedState(elapsed_time);
			this->positionX += this->velocityX;
			this->positionY += this->velocityY;
			*/
			
			AStarPathFinding path_finding(this->_map.map_width, this->_map.map_height, this->_map.tile_size, this->_map.tiles);
			path_finding.set_start_index(coords_to_index(this->_map.map_width, this->_map.tile_size, this->positionX, this->positionY));

			if (this->_gameState->getPlayerPositions().size() > 0 && this->_raycastTiles(this->_map, this, this->_gameState->getPlayerPositions()[0], 80)) {
				this->target = this->_gameState->getPlayerPositions()[0];
			}

			if (this->target == nullptr) {
				path_finding.set_end_index(0);
			}
			else {
				path_finding.set_end_index(coords_to_index(this->_map.map_width, this->_map.tile_size, this->target->positionX, this->target->positionY));
				std::cout << "Seeking player at at " << coords_to_index(this->_map.map_width, this->_map.tile_size, this->target->positionX, this->target->positionY) << " from " << coords_to_index(this->_map.map_width, this->_map.tile_size, this->positionX, this->positionY) << "\n";

			}



			path_finding.search();
			std::vector<int> path = path_finding.get_path();

			// Path is going to contain the node we're already on
			if (path.size() > 1) {
				int next_index = path.at(path.size() - 2);

				std::cout << "next index is " << next_index << "\n";

				Point next_coord = index_to_coords(this->_map.map_width, this->_map.tile_size, next_index);
				if (next_coord.x > this->positionX) {
					this->velocityX = this->maxSpeed;
				}
				else if (next_coord.x < this->positionX) {
					this->velocityX = -this->maxSpeed;
				}
				else {
					this->velocityX = 0;
				}

				if (next_coord.y > this->positionY) {
					this->velocityY = this->maxSpeed;
				}
				else if (next_coord.y < this->positionY) {
					this->velocityY = -this->maxSpeed;
				}
				else {
					this->velocityY = 0;
				}
				std::cout << "Calculating... " << this->positionX << " - " << this->velocityX << " - " << elapsed_time << "\n";
				this->positionX += this->velocityX; // (this->velocityX / 1000) * elapsed_time;
				this->positionY += this->velocityY; // (this->velocityY / 1000) * elapsed_time;
			}

			if (this->positionX < 0) {
				this->positionX = 0;
			}

			if (this->positionY < 0) {
				this->positionY = 0;
			}


			Point map_coords = index_to_coords(this->_map.map_width, this->_map.tile_size, (this->_map.map_width * this->_map.map_height) - 1);
			if (this->positionX > map_coords.x) {
				this->positionX = map_coords.x;
			}

			if (this->positionY > map_coords.y) {
				this->positionY = map_coords.y;
			}

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
	}
}

void GameMob::setPosition(float x, float y) {
	this->positionX = x;
	this->eventOriginalX = x;

	this->positionY = y;
	this->eventOriginalY = y;
}