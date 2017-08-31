#include "gamestate.h"
#include "gameclient.h"
#include <fstream>
#include "rapidjson/istreamwrapper.h"
#include <iostream>
#include "utils.h"
#include <math.h>

#define PI 3.14159265

GameState::GameState() {
	char s[25];
	gen_random(s, 24);
	s[24] = '\0';
	strcpy(this->token, s);


	// Default scripted nodes
	std::vector<EventNode> nodes;
	nodes.push_back(EventNode(0, 2));
	nodes.push_back(EventNode(3, 0));
	nodes.push_back(EventNode(0, -2));
	nodes.push_back(EventNode(-3, 0));
	this->eventNodes.push_back(nodes);

	this->loadLevel("level1");
}

GameState::~GameState() {
	std::cout << "Destroy gamestate\n";
	for (GameMob* mob : this->mobs) {
		delete mob;
	}
}

void GameState::loadLevel(std::string level) {
	std::string filename = "C:/Users/Justin/Misc Code/DungeonPuzzler/tilesets_json/" + level + ".json";
	std::ifstream file(filename.c_str());
	rapidjson::Document doc;
	rapidjson::IStreamWrapper isw(file);
	doc.ParseStream(isw);
	file.close();
	
	std::string decoded_string;
	
	decoded_string = base64_decode(std::string(doc["layers"][0]["data"].GetString()));
	for (int i = 0; i < decoded_string.size(); i = i + 4) {
		this->tiles.push_back((unsigned char) decoded_string[i]);
	}

	this->spawnPoint = std::make_pair(doc["layers"][5]["objects"][0]["x"].GetFloat(), doc["layers"][5]["objects"][0]["y"].GetFloat());
	
	int mob_index = getLayerIndex(doc["layers"], "mobs");

	rapidjson::Value mob_objects = doc["layers"][mob_index]["objects"].GetArray();
	for (rapidjson::Value::ValueIterator it = mob_objects.Begin(); it != mob_objects.End(); ++it) {
		GameMob* mob = new GameMob();
		mob->setPosition((*it)["x"].GetInt(), (*it)["y"].GetInt());
		mob->loadScript(this->eventNodes[0]);
		this->mobs.push_back(mob);
		//std::cout << (*it)["x"].GetInt() << ", " << (*it)["y"].GetInt() << "\n";
	}
}

int GameState::getLayerIndex(rapidjson::Value &layers, std::string name) {
	if (!layers.IsArray()) {
		return -1;
	}

	for (rapidjson::Value::ValueIterator it = layers.Begin(); it != layers.End(); ++it) {
		
		if ((*it)["name"].GetString() == name) {
			return it - layers.Begin();
		}
	}

	return 0;
}

void GameState::addPlayer(GamePlayer* player) {
	this->players.push_back(player);
}

GameStatus GameState::getStateUpdates() {
	GameStatus g;
	g.players = this->getPlayerPositions();
	strcpy(g.status, status);

	return g;
}

std::vector<GamePlayer*> GameState::getPlayerPositions() {
	return this->players;
}

const char * GameState::getToken() {
	return token;
}

// @todo Should be locking player so that write doesn't occur while checking for collision. Alternatively, pass collision detection into updatePosition?
bool GameState::canMove(GamePlayer* player, float x, float y) {
	int h_step = 15;
	double o = 0,
		a = 0;

	//std::cout << "Current Position: " << player->positionX << ", " << player->positionY << "\n";

	// Distance of x axis;
	o = abs(player->positionX - x);
	a = abs(player->positionY - y);
	
	//std::cout << "Distance: " << o << ", " << a << "\n";

	// Before checking path, check if final destination is valid
	if (!boxCheck(player, x, y)) {
		return false;
	}

	if (o == 0 && a == 0) {
		return true;
	}
	
	float distance = getDistance(player, x, y);
	/*
	if (fabsf(player->positionX - x) > player->maxSpeed) {
		return false;
	}


	if (fabsf(player->positionY - y) > player->maxSpeed) {
		return false;
	}
	*/

	double radians = atan(o / a) * 180 / PI;
	double path_length = sin(radians) * o;

	//std::cout << "Path Length: " << path_length << "\n";


	// For every x pixels in path, check tile
	if (path_length > h_step) {
		int tmp_h = 1;
		double new_y = 0,
			new_x = 0;

		// @todo Should reduce length so last tile isn't being double checked

		while (tmp_h < path_length) {
			new_y = player->positionY + (sin(radians) * tmp_h);
			new_x = player->positionX + (cos(radians) * tmp_h);
			if (!boxCheck(player, new_x, new_y)) {
				return false;
			}
			//std::cout << "Interpolation Check: " << new_x << " " << new_y << "\n";
			tmp_h += h_step;
		}
	}

	return true;
}

float GameState::getDistance(GamePlayer* player, float x, float y) {
	GameClient& c = player->gameClient;
	std::cout << std::to_string(player->gameClient.lastUpdateTime) << "\n";

	return 0;
}

bool GameState::boxCheck(GamePlayer* player, float x, float y) {
	if (!isWalkableTile(getTileIndex(x, y))) {
		return false;
	}

	if (!isWalkableTile(getTileIndex(x + player->width, y))) {
		return false;
	}

	if (!isWalkableTile(getTileIndex(x, y + player->height))) {
		return false;
	}

	if (!isWalkableTile(getTileIndex(x + player->width, y + player->height))) {
		return false;
	}

	return true;
}

int GameState::getTileIndex(float x, float y) {
	int row, col;
	col = floor(x / this->tileWidth);
	row = floor(y / this->tileHeight);

	return (row * this->tilesPerRow) + col;
}

bool GameState::isWalkableTile(int tile_index) {
	if (this->tiles[tile_index] > 0) {
		return true;
	}
	return false;
}

void GameState::updatePosition(int player_index, float x, float y, float velocity_x, float velocity_y, FacingDirection facing) {
	GamePlayer* player = players[player_index];
	if(canMove(player, x, y)) {
		player->updatePosition(x, y, facing);
		player->velocityX = velocity_x;
		player->velocityY = velocity_y;
	}
}

/**
 * As of current coding, collision detection assumes no angular boxes (everything is 90 degrees)
 */
void GameState::attackTarget(int player_index, float player_x, float player_y, float hitbox_x, float hitbox_y, float hitbox_rotation) {
	// Get current weapon
	int weapon_width = 10;
	int weapon_height = 10;

	float new_x = hitbox_x * cos(hitbox_rotation) - hitbox_y * sin(hitbox_rotation);
	float new_y = hitbox_y * cos(hitbox_rotation) + hitbox_x * sin(hitbox_rotation);

	// Collision box points
	// 1------2
	// |      |
	// |      |
	// 4------3
	xy_points_t hitbox_points = { {
		{ new_x, new_y },
		{ new_x + weapon_width, new_y },
		{ new_x + weapon_width, new_y + weapon_height },
		{ new_x, new_y + weapon_height }
	} };

	for (std::vector<GameMob*>::iterator it = this->mobs.begin(); it != this->mobs.end(); it++) {
		float mob_x = (*it)->positionX;
		float mob_y = (*it)->positionY;
		int width = (*it)->width;
		int height = (*it)->height;

		xy_points_t mob_points = { {
			{ mob_x, mob_y },
			{ mob_x + weapon_width , mob_y },
			{ mob_x + weapon_width, mob_y + weapon_height },
			{ mob_x, mob_y + weapon_height }
		} };

		intersects(mob_points, hitbox_points);

	}
}



void GameState::update(double elapsed_time) {
	std::cout << "GameState update\n";
	if (this->mobs.size() == 0) {
		return;
	}

	for (std::vector<GameMob*>::iterator it = this->mobs.begin(); it != this->mobs.end(); ++it) {
		(*it)->update(elapsed_time);
	}
}