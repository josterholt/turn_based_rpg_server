#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#pragma once

#include <vector>
#include "gameplayer.h"
#include "gamemob.h"
#include <array>
#include "utils.h"
#include "rapidjson/document.h"
#include "shared_structures.hpp"

class GamePlayer;
class GameMob;
struct EventNode;



struct GameStatus {
	char status[128];
	std::vector<GamePlayer*> players;
	GameStatus() {

	}

	GameStatus(std::vector<GamePlayer*> players, const char* status) {
		status = status;
		players = players;
	}

};

class GameState {
	bool loadLevel(std::string level);
	std::vector<unsigned int>* tiles;
	int tileWidth = 32;
	int tileHeight = 32;
	int tilesPerRow = 100;

	int cleanupInterval = 5000; // Every 5 seconds
	float cleanupTimer = 0;

	float getDistance(GamePlayer* player, float x, float y) const;
public:
	GameState();
	~GameState();
	std::vector<GamePlayer*> players;
	std::vector<GameMob*> mobs;
	std::vector<std::vector<EventNode>> eventNodes;

	std::vector<xy_points_t> hitboxes;

	const std::string token;
	const int gameID = 0;
	const char* status;

	boost::shared_mutex positionMutex;

	static const std::string GenerateToken() {
		char s[25];
		gen_random(s, 24);
		s[24] = '\0';
		return std::string(s);
	}

	void addPlayer(GamePlayer* player);
	void addMOB(GameMob* mob);
	void moveToSpawn(GamePlayer* player);
	GameStatus getStateUpdates() const;
	std::vector<GamePlayer*> getPlayerPositions() const;
	const std::string& getToken() const;
	int getLayerIndex(rapidjson::Value &layers, std::string name) const;
	void updatePosition(int player_index, float x, float y, float velocity_x, float velocity_y, FacingDirection facing);
	void attackTarget(int player_index, float player_x, float player_y, float hitbox_x, float hitbox_y, int player_facing);
	bool canMove(GamePlayer* player, float x, float y) const;
	std::vector<unsigned int>* getTiles();
	int getTileIndex(float x, float y) const;
	bool boxCheck(GamePlayer* player, float x, float y) const;
	bool isWalkableTile(int tile_index) const;
	std::pair<float, float> spawnPoint;
	void update(double elapsed_item);
};
#endif