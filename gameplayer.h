#ifndef _GAMEPLAYER_H_
#define _GAMEPLAYER_H_

#include "eventmanager.h"
#include "gamestate.h"
#include "gameunit.h"

class GameClient;
class GameState;

class GamePlayer : public GameUnit {
public:
	GamePlayer(GameClient& client);
	GameClient& gameClient;
	int userID = 0;
	GameState* game;
	EventManager eventMgr;
	int playerIndex = 0;
	int damage = 5;
};
#endif