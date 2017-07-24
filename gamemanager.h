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
	static GameManager& getInstance()
	{
		static GameManager instance;
		return instance;
	}
	GameState* createGame();
	GameState* joinGame(std::string key);
	GameState* getGame(std::string key);
	void addGame(std::string key, GameState * gamestate);
	void update(double time_elapsed);
};
#endif