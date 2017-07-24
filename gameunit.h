#ifndef _GAMEUNIT_H_
#define _GAMEUNIT_H_
#include "boost/thread.hpp"

enum FacingDirection {
	LEFT,
	UP,
	RIGHT,
	DOWN
};

struct UnitState {
	float positionX;
	float positionY;
	float velocityX;
	float velocityY;
	FacingDirection facing;
};

class GameUnit {
	virtual void update(double elapsed_time);
public:
	float positionX;
	float positionY;
	float velocityX;
	float velocityY;
	int width;
	int height;
	float health;
	float maxSpeed;
	float speed;
	char token[25];

	FacingDirection facing;
	bool stunned;

	boost::shared_mutex positionMutex;

	GameUnit();
	void updatePosition(float x, float y, FacingDirection facing);
	void moveToSpawn(float x, float y, FacingDirection facing);
	UnitState getFullState();
};
#endif
