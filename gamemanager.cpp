#include "gamemanager.h"

GameManager::~GameManager() {
	for (std::map<std::string, GameState*>::iterator it = this->games.begin(); it != this->games.end(); it++) {
		delete it->second;
	}
}

GameState* GameManager::getGame(std::string key) {
	if (this->games.find(key) == this->games.end()) {
		return nullptr;
	}
	return this->games[key];
}

void GameManager::addGame(std::string key, GameState* gamestate) {
	this->games.insert(std::make_pair(key, gamestate));
}

void GameManager::update(double elapsed_time) {
	for (std::map<std::string, GameState*>::iterator it = this->games.begin(); it != this->games.end(); it++) {
		it->second->update(elapsed_time);
	}
}