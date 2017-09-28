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
	std::cout << "Adding game " << key << "\n";
	this->games.insert(std::make_pair(key, gamestate));
}

void GameManager::update(double elapsed_time) {
	//std::cout << "Games " << this->games.size() << "\n";
	for (std::map<std::string, GameState*>::iterator it = this->games.begin(); it != this->games.end(); it++) {
		it->second->update(elapsed_time);
	}
}

void GameManager::destroyGame(char* token) {
	//std::cout << "Destroy game " << token << "\n";
	std::map<std::string, GameState*>::iterator it = this->games.find(token);
	if (it == this->games.end()) {
		std::cout << "Unable to find game for delete\n";
	} else {
		GameState* game_state = it->second;
		std::cout << "Game has " << game_state->players.size() << " players\n";
		if (game_state->players.size() == 0) {
			std::cout << "Destroying game...\n";
			this->games.erase(token);
		}
	}
}