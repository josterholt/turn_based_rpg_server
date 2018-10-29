#ifndef _GAMEUNIT_H_
#define _GAMEUNIT_H_
#include "boost/thread.hpp"
#include <string>

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
	GameUnit *target;

public:
	std::string name;
	char token[25];
	std::string type;
	std::string image;

	// Leveling
	int level = 1;
	int experience = 0;
	int experienceGiven = 0;

	// Vital
	int baseHealth = 150;
	int _health = 0;
	int baseMana = 0;
	int _mana = 0;
	int baseStamina = 10;
	int baseDefense = 2;
	int _baseDodge = 1;

	// Multipliers
	int staminaPerLevelMultiplier = 0.5;
	float staminaToHealthMultiplier = 1.5f;
	float _agilityDodgePerPoint = 0.8f;

	// Positional / State
	float positionX;
	float positionY;
	float velocityX;
	float velocityY;
	int width;
	int height;
	float health;
	float maxSpeed;
	float speed;
	bool isAttacking = false;
	

	FacingDirection facing;
	bool stunned;
	bool canMove = true;

	boost::shared_mutex positionMutex;

	GameUnit();
	GameUnit(std::string name, int x, int y, int width, int height);
	int getMaxHealth() const;
	int GameUnit::getStamina() const;

	bool setTarget(GameUnit *target);
	GameUnit* getTarget() const;

	void updatePosition(float x, float y, FacingDirection facing);
	void moveToSpawn(float x, float y, FacingDirection facing);
	UnitState getFullState(); // @todo Can't make this const because it changes the mutex. Is this the best implementation?
};
#endif
