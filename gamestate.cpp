#include "gamestate.h"
#include "gameclient.h"
#include <fstream>
#include "rapidjson/istreamwrapper.h"
#include <iostream>
#include "utils.h"
#include <math.h>
#include "npcs.hpp"

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
	std::cout << "Loading level...\n";
	std::string filename = "C:/Users/Justin/Misc Code/Game/combat_system/src/assets/tiled_map/" + level + ".json";
	std::ifstream file(filename.c_str());

	if ((file.rdstate() & std::ifstream::failbit) != 0) {
		std::cout << "Error opening file\n";
		return;
	}

	rapidjson::Document doc;
	rapidjson::IStreamWrapper isw(file);
	doc.ParseStream(isw);
	file.close();

	std::cout << doc.GetParseError() << "\n";
	
	std::string decoded_string;
	rapidjson::Value tiles = doc["layers"][0]["data"].GetArray();
	for (rapidjson::Value::ValueIterator it = tiles.Begin(); it != tiles.End(); ++it) {
		this->tiles.push_back(it->GetInt());
	}

	/*
	//decoded_string = std::string(doc["layers"][0]["data"].GetString());
	for (int i = 0; i < tiles.size(); i = i + 4) {
		this->tiles.push_back((unsigned char) decoded_string[i]);
	}
	*/

	//this->spawnPoint = std::make_pair(doc["layers"][5]["objects"][0]["x"].GetFloat(), doc["layers"][5]["objects"][0]["y"].GetFloat());
	
	/*
	int mob_index = getLayerIndex(doc["layers"], "mobs");

	rapidjson::Value mob_objects = doc["layers"][mob_index]["objects"].GetArray();
	for (rapidjson::Value::ValueIterator it = mob_objects.Begin(); it != mob_objects.End(); ++it) {
		std::string mob_name = (*it)["properties"]["type"].GetString();
		if (std::find(NPC_LIST.begin(), NPC_LIST.end(), mob_name) != NPC_LIST.end()) {
			if ("mob_name" == "skeleton") {
				GameMob* mob = new GameMob();
				mob->health = skeleton.health;
				
				mob->width = skeleton.width;
				mob->height = skeleton.height;
				mob->setPosition((*it)["x"].GetInt(), (*it)["y"].GetInt());
				mob->loadScript(this->eventNodes[0]);
				this->mobs.push_back(mob);
				//std::cout << (*it)["x"].GetInt() << ", " << (*it)["y"].GetInt() << "\n";
			}
		}		
	}
	*/
	std::cout << "Level loaded\n";
	std::cout << "Num mobs: " << this->mobs.size() << "\n";
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

void GameState::addMOB(GameMob* mob) {
	this->mobs.push_back(mob);
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
	std::cout << "Distance: " << std::to_string(player->gameClient.lastUpdateTime) << "\n";

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
	if (tile_index < 0) {
		return false;
	}

	if (tile_index > this->tiles.size()) {
		return false;
	}

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
void GameState::attackTarget(int player_index, float player_x, float player_y, float hitbox_x, float hitbox_y, int player_facing) {
	// Get current weapon
	int weapon_width = 10;
	int weapon_height = 30;

	//float new_x = player_x + (hitbox_x * cos(hitbox_rotation) - hitbox_y * sin(hitbox_rotation));
	//float new_y = player_y + (hitbox_y * cos(hitbox_rotation) + hitbox_x * sin(hitbox_rotation));

	// Collision box points
	// 1------2
	// |      |
	// |      |
	// 4------3

	point_t p1, p2, p3, p4;

	if(player_facing == 3) { // Facing UP
		float x_offset = 0.0f;
		float y_offset = 0.0f;
		p1 = { player_x, player_y - weapon_width };
		p2 = { player_x + weapon_height, player_y - weapon_width };
		p3 = { player_x + weapon_height, player_y };
		p4 = { player_x, player_y };
	}
	else if (player_facing == 1) { // Facing LEFT
		p1 = { player_x - weapon_width, player_y };
		p2 = { player_x, player_y };
		p3 = { player_x, player_y + weapon_height };
		p4 = { player_x - weapon_width, player_y + weapon_height };
	}
	else if (player_facing == 4) { // Facing DOWN
		float x_offset = 0.0f;
		float y_offset = 48.0f;
		p1 = { player_x, player_y + y_offset };
		p2 = { player_x + weapon_height, player_y + y_offset };
		p3 = { player_x + weapon_height, player_y + y_offset + weapon_width };
		p4 = { player_x, player_y + y_offset + weapon_width };
	}
	else { // Facing RIGHT
		float x_offset = 32.0f;
		float y_offset = 0.0f;
		p1 = { player_x + x_offset, player_y };
		p2 = { player_x + x_offset + weapon_width, player_y };
		p3 = { player_x + x_offset + weapon_width, player_y + weapon_height };
		p4 = { player_x + x_offset, player_y + weapon_height };
	}

	xy_points_t hitbox_points = {
		{ p1[0], p1[1] },
		{ p2[0], p2[1] },
		{ p3[0], p3[1] },
		{ p4[0], p4[1] }
	};

	//hitboxes.push_back(hitbox_points);
	int mob_width = 32; // temporary
	int mob_height = 48; // temporary

	for (std::vector<GameMob*>::iterator it = this->mobs.begin(); it != this->mobs.end(); it++) {
		float mob_x = (*it)->positionX;
		float mob_y = (*it)->positionY;
		int width = (*it)->width;
		int height = (*it)->height;


		xy_points_t mob_points = { {
			{ mob_x, mob_y },
			{ mob_x + mob_width , mob_y },
			{ mob_x + mob_width, mob_y + mob_height },
			{ mob_x, mob_y + mob_height }
		} };

		bool intersects_mob = false;

		if (hitbox_points[0][0] < mob_x + mob_width &&
			hitbox_points[1][0] > mob_x &&
			hitbox_points[0][1] < mob_y + mob_height &&
			hitbox_points[3][1] > mob_y) {
			intersects_mob = true;
		}

		if (!intersects_mob) {
			if (mob_x < hitbox_points[1][0] &&
				mob_x + mob_width > hitbox_points[0][0] &&
				mob_y < hitbox_points[2][1] &&
				mob_y + mob_height > hitbox_points[0][1]) {
				intersects_mob = true;
			}
		}


		if (intersects_mob) {
			std::cout << "#### MOB hit ####\n";
			(*it)->health -= players[player_index]->damage;
		}
	}
}

void GameState::update(double elapsed_time) {
	//std::cout << "GameState update\n";
	//std::cout << "Update mobs " << this->mobs.size() << "\n";
	this->cleanupTimer += elapsed_time;
	if (this->cleanupTimer > this->cleanupInterval) {
		this->cleanupTimer = 0;

		std::cout << "Cleaning up mobs...\n";
		
		for (std::vector<GameMob*>::iterator it = this->mobs.begin(); it != this->mobs.end(); ) {
			if ((*it)->health == 0) {
				std::cout << "Cleaning up mob\n";
				it = this->mobs.erase(it);
			}
			else {
				++it;
			}
		}
	}

	int alive_mob_count = 0;
	for(std::vector<GameMob*>::iterator it = this->mobs.begin(); it != this->mobs.end(); it++) {
		if ((*it)->health > 0) {
			alive_mob_count += 1;
		}
	}

	if (alive_mob_count == 0) {
		GameMob* mob = new GameMob();
		mob->health = 20;
		// Get player position
		int player_x = this->players[0]->positionX;
		int player_y = this->players[0]->positionY;

		// Spawn within x pixels
		// Make sure spawn point is on a tile
		int offset_x = rand() % 50 + 1,
			offset_y = rand() % 50 + 1;;

		int mob_x = player_x + 10,
			mob_y = player_y + 10;

		srand(time(NULL));
		// Brute force spawn generation
		while (!this->isWalkableTile(this->getTileIndex(mob_x, mob_y))) {
			offset_x = rand() % 50 + 1;
			offset_y = rand() % 50 + 1;

		}
		std::cout << "Setting position " << mob_x << ", " << mob_y << "\n";
		mob->setPosition(mob_x, mob_y);

		mob->loadScript(this->eventNodes[0]);
		std::cout << "Adding mob " << mob->token << " at " << mob->positionX << ", " << mob->positionY << "\n";
		this->addMOB(mob);
	}

	for (std::vector<GameMob*>::iterator it = this->mobs.begin(); it != this->mobs.end(); ++it) {
		(*it)->update(elapsed_time);
	}
}