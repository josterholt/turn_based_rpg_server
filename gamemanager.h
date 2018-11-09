#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include "gamestate.h"
#include <map>
#include <string>

class GameState;

class GameManager {
	std::map<std::string, GameState*> games;
	GameManager() {};
public:
	GameManager(GameManager const&) = delete;
	void operator=(GameManager const&) = delete;
	~GameManager();

	const std::string& handleGameRequest(const std::string& token);

	static GameManager& getInstance()
	{
		static GameManager instance;
		return instance;
	}
	//GameState* createGame();
	//GameState* joinGame(std::string key);

	GameState* getGame(const std::string& key) const;
	void addGame(const std::string key, GameState * gamestate);
	void update(double time_elapsed);
	void destroyGame(const std::string& token);
	void destroyAllGames();

	static size_t getGameCount() {
		return GameManager::getInstance().games.size();
	}
};
#endif