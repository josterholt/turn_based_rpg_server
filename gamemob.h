#ifndef _GAMEMOB_H_
#define _GAMEMOB_H_
#include "gameunit.h"
#include <vector>

struct EventNode {
	float velocityX = 0;
	float velocityY = 0;

	EventNode(float x, float y)
		: velocityX(x), velocityY(y) {}
};

class GameMob : public GameUnit {
private:
	int eventNodeIndex = 0;
	std::vector<EventNode> eventNodes;
	float eventOriginalX = 0;
	float eventOriginalY = 0;

public:
	void loadScript(std::vector<EventNode> &nodes);
	void update(double elapsed_time);
	void processScriptedState(double elapsedTime);
	void setPosition(float x, float y);
};
#endif