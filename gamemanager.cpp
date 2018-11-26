#include "gamemanager.h"

GameManager::~GameManager() {
	for (std::map<std::string, GameState*>::iterator it = this->games.begin(); it != this->games.end(); it++) {
		delete it->second;
	}
}

GameState* GameManager::getGame(const std::string& key) const {
	if (this->games.find(key) == this->games.end()) {
		return nullptr;
	}
	
	std::map<std::string, GameState *>::const_iterator it = this->games.find(key);
	if (it == this->games.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

const std::string& GameManager::handleGameRequest(const std::string& key) {
	GameState* game_state;
	if (key == "" || key == "NEW_GAME") {
		game_state = new GameState();
		this->addGame(game_state->token, game_state);
	}
	else {
		game_state = GameManager::getInstance().getGame(key);

		// Can't find game_state, return empty string
		if (!game_state) {
			return std::string("");
		}
	}
	return game_state->token;
}

void GameManager::addGame(std::string key, GameState* gamestate) {
	this->games.insert(std::make_pair(key, gamestate));
}

void GameManager::update(double elapsed_time) {
	//std::cout << "Games " << this->games.size() << "\n";
	for (std::map<std::string, GameState*>::iterator it = this->games.begin(); it != this->games.end(); it++) {
		it->second->update(elapsed_time);
	}
}

void GameManager::destroyGame(const std::string& token) {
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

void GameManager::destroyAllGames() {
	this->games.clear();
}