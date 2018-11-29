#include "gameclient.h"
#include "gamemob.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <math.h>
#include <iostream>
#include <istream>
#include "src/protobuf/connect.pb.h"
#include "src/protobuf/playerUpdate.pb.h"

/**
 * @todo Game creation should be moved to gamemanager. gameclient is intended to represent the frontend client interface
 */
GameClient::GameClient() : player(*this) {
	player.userID = 1;
	player.width = 32;
	player.height = 48;
	player.maxSpeed = 100;

	time(&lastUpdateTime);

	// Placing player in a single isolated game for now
	//this->initPlayer(1);
	//this->currentGameInstance = this->createGame();
	//this->player.moveToSpawn(this->currentGameInstance->spawnPoint.first, this->currentGameInstance->spawnPoint.second, FacingDirection::UP);

}

bool GameClient::handleConnectionMessage(gamemessages::Connect connect_message) {
	// @todo hanleGameRequest shouldn't be in if block
	if (connect_message.gametoken() == "" || connect_message.gametoken() == "NEW_GAME") {
		const std::string& game_token = GameManager::getInstance().handleGameRequest(connect_message.gametoken());
		this->currentGameInstance = GameManager::getInstance().getGame(game_token);
		this->currentGameInstance->addPlayer(&this->player);
	}
	else {
		this->currentGameInstance = GameManager::getInstance().getGame(connect_message.gametoken());
	}

	// If no game instance was assigned, return false
	if (this->currentGameInstance == nullptr) {
		return false;
	}

	return true;
}

void GameClient::open() {
	// Add instance to client pool, create client ID
}

void GameClient::initPlayer(int user_id) {
	this->player.userID = user_id;
}

GameState* GameClient::createGame() {
	GameState* game_state = new GameState();
	GameManager::getInstance().addGame(game_state->token, game_state);
	this->currentGameInstance = game_state;
	this->currentGameInstance->addPlayer(&this->player);
	return game_state;
}

GameState* GameClient::joinGame(std::string key) {
	GameState* game_state = GameManager::getInstance().getGame(key);
	this->currentGameInstance = game_state;
	int player_index;
	//player_index = 1; // Player 2 index. Hardcoded for now
	this->currentGameInstance->addPlayer(&this->player);
	return game_state;
}

/**
 * Player is leaving game. Delete game instance if no more players exist
 */
void GameClient::leaveGame() {
	GamePlayer* player = &this->player;
	std::cout << "Remove: " << player << "\n";
	const std::string& game_token = this->currentGameInstance->token;

	if (this->currentGameInstance != nullptr) {
		int existing_players = 0;
		std::vector<GamePlayer*>::iterator p_it = std::find(this->currentGameInstance->players.begin(), this->currentGameInstance->players.end(), player);
		if (p_it != this->currentGameInstance->players.end()) {
			this->currentGameInstance->players.erase(p_it);
		}

		GameManager::getInstance().destroyGame(game_token);
	}

}

GameState* GameClient::getGame() const {
	return this->currentGameInstance;
}

void GameClient::onMessage() {
	// Writes message
}

/*
std::istream& operator>>(std::istream& is, char* input) {
	std::istream::sentry s(is);
	if (s) while (is.good()) {
		char c = is.get();
		if (!std::isdigit(c, is.getloc())) {
			std::cout << c << "\n";
		}
	}
	return is;
}
*/

void GameClient::processRequest(char* message, size_t len) {
	try {
		/*
		char* tempBuffer = new char[256];
		memcpy(tempBuffer, "", sizeof(tempBuffer));

		std::istringstream iss(message);
		int i;
		char a[3];
		char b[5];
		
		iss >> i;
		iss >> a;
		iss >> b;
	
		//iss.read(i, sizeof(i));
		//iss.read(a, sizeof(a));
		//iss.read(b, sizeof(b));
		std::cout << "(" << sizeof(i) << ") " << i << "\n";
		std::cout << "(" << sizeof(a) << ") " << a << "\n";
		std::cout << "(" << sizeof(b) << ") " << b << "\n";
		*/
		/*
		rapidjson::Document d;
		d.Parse(message, len);
		if (!d.HasParseError()) {

			rapidjson::Value& actions = d["actions"];
			std::string action;

			rapidjson::Value::ConstMemberIterator it;
			it = d.FindMember("action");


			for (rapidjson::SizeType i = 0; i < actions.Size(); i++) {
				action = actions[i]["action"].GetString();
				rapidjson::Value& val = actions[i];

				if (action.compare("createGame") == 0) {
					this->instantiateGameInstance(action, actions[i]);
				}
				else if (action.compare("joinGame") == 0) {

				}
				else if (action.compare("updatePlayer") == 0) {
					this->updatePlayerState(action, actions[i]);
				}
				else if (action.compare("playerAttack") == 0) {
					this->attackTarget(actions[i]);
				}
			}
		}
		*/
	}
	catch (const std::exception& e) {
		std::cout << e.what();
	}
	catch (...) {
		// Don't want the server to crash due to a ill-formed request
		std::cout << "An error occurred while process request\n";
		std::cout << message << "\n";
	}
}

bool GameClient::instantiateGameInstance(std::string &action, rapidjson::Value &doc) {
	std::cout << "Creating new game with player...\n";
	rapidjson::Value::ConstMemberIterator it;
	it = doc.FindMember("gameId");
	if (it == doc.MemberEnd()) {
		this->initPlayer(1);
		this->currentGameInstance = this->createGame();
		this->player.moveToSpawn(this->currentGameInstance->spawnPoint.first, this->currentGameInstance->spawnPoint.second, FacingDirection::UP);
	}
	else {
		this->initPlayer(2);
		this->currentGameInstance = this->joinGame(it->value.GetString());
		this->player.moveToSpawn(this->currentGameInstance->spawnPoint.first, this->currentGameInstance->spawnPoint.second, FacingDirection::UP);
	}

	if (!this->routineQueue.push(GameClient::routines::INIT)) {
		std::cout << "Unable to add routine\n";
	}

	return true;
}

bool GameClient::updatePlayerState(gamemessages::PlayerUpdate update_message) {
	// @todo facing direction should be updated by client
	this->currentGameInstance->updatePosition(this->player.playerIndex, update_message.player().position().x(), update_message.player().position().y(), 0, 0, FacingDirection::UP);
	return true;
}

bool GameClient::attackTarget(rapidjson::Value &doc) {
	float player_x = doc["player"]["x"].GetFloat();
	float player_y = doc["player"]["y"].GetFloat();
	int player_facing = doc["player"]["facing"].GetInt();

	float hitbox_x = doc["hitbox"]["x"].GetFloat();
	float hitbox_y = doc["hitbox"]["y"].GetFloat();
	float hitbox_rotation = doc["hitbox"]["rotation"].GetFloat();
	//std::cout << hitbox_x << ", " << hitbox_y << ", " << hitbox_rotation << "\n";

	this->currentGameInstance->attackTarget(this->player.playerIndex, player_x, player_y, hitbox_x, hitbox_y, player_facing);
	return true;
}

void GameClient::onClose() {
	// Remove instance from client pool
}

void GameClient::addOutgoingMessage(std::string message) {
	char tmp_str[255];
	strncpy(tmp_str, message.c_str(), sizeof(tmp_str));
	tmp_str[sizeof(tmp_str) - 1] = 0;

	
	//this->outgoingMessages.push(tmp_str);
}

/**
 * @todo cleanup; not sure why this is returning an empty string
 */
std::string GameClient::getNextOutgoingMessage() {
	std::string tmp_string;
	//this->outgoingMessages.pop(tmp_string);
	return tmp_string;
}

/**
 * {
 *     gameToken
 *     playerToken
 *     playerIndex
 *     players: [{
 *         position: [0, 1]
 *         velocity: [0, 1]
 *         facing: 0
 *     }]
 *     mobs
 * }
 * Returns:
 * std::string or "" (empty string)
 */
std::string GameClient::generateInit() {
	if (this->currentGameInstance != nullptr) {
		/*
		rapidjson::Document root;
		root.SetObject();
		root.AddMember("status", "OK", root.GetAllocator());

		rapidjson::Value data;
		data.SetObject();

		rapidjson::Value game_token;
		data.AddMember("gameToken", rapidjson::Value(this->currentGameInstance->token, strlen(this->currentGameInstance->token)), root.GetAllocator());
		data.AddMember("playerIndex", rapidjson::Value(0), root.GetAllocator());
		data.AddMember("playerToken", rapidjson::Value(this->player.token, strlen(this->player.token)), root.GetAllocator());
		
		rapidjson::Value players;
		players.SetArray();
		
		// Loop
		std::vector<GamePlayer*>* current_players = &this->currentGameInstance->players;
		for (std::vector<GamePlayer*>::iterator it = current_players->begin(); it != current_players->end(); ++it) {
			GamePlayer* game_player = *it;

			rapidjson::Value player;
			player.SetObject();

			rapidjson::Value position;
			position.SetArray();
			position.PushBack(game_player->positionX, root.GetAllocator());
			position.PushBack(game_player->positionX, root.GetAllocator());
			
			player.AddMember("position", position, root.GetAllocator());

			rapidjson::Value velocity;
			velocity.SetArray();
			velocity.PushBack(game_player->velocityX, root.GetAllocator());
			velocity.PushBack(game_player->velocityY, root.GetAllocator());

			player.AddMember("velocity", velocity, root.GetAllocator());

			player.AddMember("facing", rapidjson::Value(game_player->facing), root.GetAllocator());

			players.PushBack(player, root.GetAllocator());
		}
		data.AddMember("players", players, root.GetAllocator());
		*/
		/**
		* Begin loop through mobs
		*/
		/*
		rapidjson::Value mob_array;
		mob_array.SetArray();
		for (GameMob* game_mob : this->currentGameInstance->mobs) {
			rapidjson::Value mob;
			mob.SetObject();

			rapidjson::Value position;
			position.SetArray();
			position.PushBack(game_mob->positionX, root.GetAllocator());
			position.PushBack(game_mob->positionY, root.GetAllocator());

			mob.AddMember("position", position, root.GetAllocator());
			mob_array.PushBack(mob, root.GetAllocator());
		}
		data.AddMember("mobs", mob_array, root.GetAllocator());
		*/
		/**
		* End loop through mobs
		*/
		/*
		data.AddMember("action", rapidjson::Value("INIT"), root.GetAllocator());
		root.AddMember("data", data, root.GetAllocator());

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		root.Accept(writer);
		return std::string(buffer.GetString());
		*/

	}
	return "";
}

/**
 * Returns:
 * std::string or "" (empty string)
 */
gamemessages::PositionUpdate GameClient::generatePositionUpdate() {
	if (this->currentGameInstance == nullptr) {
		throw "Game instance does not exist";
	}
	std::vector<GamePlayer*> players = this->currentGameInstance->getPlayerPositions();

	gamemessages::PositionUpdate proto_message;
	
	
	gamemessages::Unit *unit = proto_message.add_players();

	unit->mutable_position()->set_x(players[0]->positionX);
	unit->mutable_position()->set_y(players[0]->positionY);
	unit->mutable_velocity()->set_x(players[0]->velocityX);
	unit->mutable_velocity()->set_y(players[0]->velocityY);
	unit->set_facing(players[0]->facing);

	return proto_message;
}