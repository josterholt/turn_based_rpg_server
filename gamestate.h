#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#pragma once

#include <vector>
#include "gameplayer.h"
#include "gamemob.h"
#include <array>
#include "utils.h"
#include "rapidjson/document.h"

class GamePlayer;

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
	void loadLevel(std::string level);
	std::vector<unsigned int> tiles;
	int tileWidth = 32;
	int tileHeight = 32;
	int tilesPerRow = 100;

	float getDistance(GamePlayer* player, float x, float y);
public:
	GameState();
	~GameState();
	std::vector<GamePlayer*> players;
	std::vector<GameMob*> mobs;
	std::vector<std::vector<EventNode>> eventNodes;

	char token[25];
	const int gameID = 0;
	const char* status;

	boost::shared_mutex positionMutex;

	void addPlayer(GamePlayer* player);
	GameStatus getStateUpdates();
	std::vector<GamePlayer*> getPlayerPositions();
	const char * getToken();
	int GameState::getLayerIndex(rapidjson::Value &layers, std::string name);
	void updatePosition(int player_index, float x, float y, float velocity_x, float velocity_y, FacingDirection facing);
	bool canMove(GamePlayer* player, float x, float y);
	int getTileIndex(float x, float y);
	bool GameState::boxCheck(GamePlayer* player, float x, float y);
	bool isWalkableTile(int tile_index);
	std::pair<float, float> spawnPoint;
	void update(double elapsed_item);
};
#endif