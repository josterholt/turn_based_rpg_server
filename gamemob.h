#ifndef _GAMEMOB_H_
#define _GAMEMOB_H_
#include "gameunit.h"
#include <vector>
#include "shared_structures.hpp"
#include "gamestate.h"

class GameState;

struct EventNode {
	float velocityX = 0;
	float velocityY = 0;

	EventNode(float x, float y)
		: velocityX(x), velocityY(y) {}
};

struct BaseAttributes {
	int strength = 5;
	int intellect = 5;
	int agility = 5;
	int stamina = 5;
};

struct Attribute {
	std::string name;
	int value;

	Attribute(std::string _name, int _value) : name(_name), value(_value) { }
};

struct Item {
	std::string name;
	Item(std::string _name) : name(_name) { }
};


struct Gear : Item {
	std::string slot;
	int attack;
	int defense;
	std::vector<Attribute> attributes;

	Gear(std::string _name, std::string _slot, int _attack, int _defense) : Item(_name), slot(_slot), attack(_attack), defense(_defense) {
	
	}

	void addAttribute(std::string name, int value) {
		this->attributes.push_back(Attribute(name, value));
	}
};

struct UnitGearSlots {
	Gear HEAD;
	Gear SHOULDERS;
	Gear WRISTS;
	Gear HANDS;
	Gear CHEST;
	Gear LEGS;
	Gear FEET;
	Gear NECKLACE;
	Gear RING1;
	Gear RING2;
};

class GameMob : public GameUnit {
private:
	int eventNodeIndex = 0;
	std::vector<EventNode> eventNodes;
	float eventOriginalX = 0;
	float eventOriginalY = 0;

	std::string id;
	std::string name;
	std::string image;
	std::string type;

	// Map
	struct GameMap _map;
	bool _raycastTiles(GameMap map, GameUnit* source, GameUnit* target, int distance);

	// Movement timing
	float time_elapsed = 0;

	// Leveling
	int level = 1;
	int experience = 0;
	int experienceGiven = 0;

		// Vital
	int baseHealth = 150; // Possibly abstract this out
	int _health = 0;
	int baseMana = 120; // Possibly abstract this out
	int _mana = 0;
	int baseStamina = 10;
	int baseDefense = 2;
	int _baseDodge = 1;

	// Multipliers
	float staminaPerLevelMultiplier = 0.5; // Need to abstract this out
	float staminaToHealthMultiplier = 1.5;
	float _agilityDodgePerPoint = 0.8;

	GameState* _gameState;
	// Positional / State
	GameUnit* target = nullptr;
	

	// Ratings
	float missChance = 0.05;
	float critChance = 0.1; // Might be a little high?
	int hitRating;
	int dodgeRating;

		// Gear / Equipment
	int _gearDefense = 0;
	UnitGearSlots *gear;

		// Inventory
	//inventory:Item[] = [];
	int gold;

public:
	GameMob(GameState* game_state);
	bool isAttacking = false;

	// Set map/tiles
	void setMap(GameMap game_map);

	void loadScript(std::vector<EventNode> &nodes);
	void update(double elapsed_time);
	void processScriptedState(double elapsedTime);
	void setPosition(float x, float y);

	// Positional / State
	int x = 0; // May need to be broken out
	int y = 0; // May need to be broken out
	int width = 0;
	int height = 0;
	int facing = 0; // 0 - right, 1 - down, 2 - left, 3 - up
	bool canMove = true;

};
#endif