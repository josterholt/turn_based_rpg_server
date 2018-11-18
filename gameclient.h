#ifndef _GAMECLIENT_H_
#define _GAMECLIENT_H_

#include <iostream>
#include <sstream>
#include <rapidjson/document.h>

#include "gamestate.h"
#include "gamemanager.h"
#include "gameplayer.h"
#include "src\protobuf\playerUpdate.pb.h"
#include "src\protobuf\connect.pb.h"

#include <time.h>

#include "boost/lockfree/queue.hpp"


class GameState;

class GameClient {
	GameState* currentGameInstance = nullptr;
	GamePlayer player;

	//boost::lockfree::queue<const char[255]> outgoingMessages;
	void addOutgoingMessage(std::string message);
public:
	GameClient();

	int playerIndex;
	std::stringstream message;
	time_t lastUpdateTime;
	enum routines {
		INIT
	};

	boost::lockfree::queue<routines> routineQueue{ 10 };

	void open(); // Add instance to client pool, create client ID
	void onClose(); // Remove instance from client pool

	void initPlayer(int user_id);
	bool instantiateGameInstance(std::string &action, rapidjson::Value &doc);
	void processRequest(char* message, size_t len); // processes request
	bool handleConnectionMessage(gamemessages::Connect connect_message);

	GameState* createGame(); // Return token
	GameState* joinGame(std::string key); // Return game
	void leaveGame();
	GameState* getGame() const; // Return game

	bool GameClient::updatePlayerState(gamemessages::PlayerUpdate update_message);
	bool GameClient::attackTarget(rapidjson::Value &doc);

	
	gamemessages::PositionUpdate generatePositionUpdate();
	std::string generateInit();

	std::string getNextOutgoingMessage(); // @todo optimization of moving and deleting the string
	void onMessage(); // Writes message


};
#endif
