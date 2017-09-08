#include "gameclient.h"
#include "gamemob.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <math.h>


GameClient::GameClient() : player(*this) {
	player.width = 32;
	player.height = 48;
	player.maxSpeed = 100;

	time(&lastUpdateTime);
}

void GameClient::open() {
	// Add instance to client pool, create client ID
}

void GameClient::initPlayer(int user_id) {
	this->player.userID = user_id;
}

GameState* GameClient::createGame() {
	GameState* game_state = new GameState();
	GameManager::getInstance().addGame("TMP_KEY", game_state);
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
	if (this->currentGameInstance != nullptr) {
		int existing_players = 0;
		for (int i = 0; i < this->currentGameInstance->players.size(); i++) {
			if (this->currentGameInstance->players[i] == player) {
				this->currentGameInstance->players[i] = nullptr;
			}
			else if (this->currentGameInstance->players[i] != nullptr) {
				existing_players += 1;
			}
		}

		if (existing_players == 0) {
			delete this->currentGameInstance;
		}
	}
	/*
	for (std::array<GamePlayer*, 2>::iterator it = this->currentGameInstance->players.begin(); it != this->currentGameInstance->players.end(); ++it) {
		if (player == *it) {
			std::cout << "Foo\n";
			*it = nullptr;
		}
	}
	*/
}

GameState* GameClient::getGame() {
	return this->currentGameInstance;
}

void GameClient::onMessage() {
	// Writes message
}

void GameClient::processRequest(char* message, size_t len) {
	try {
		rapidjson::Document d;
		d.Parse(message, len);

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

bool GameClient::updatePlayerState(std::string &action, rapidjson::Value &doc) {
	//std::pair<float, float> p = std::make_pair(doc["player"]["position"][0].GetFloat(), doc["player"]["position"][1].GetFloat());
	//this->player.position = p;
	// @todo facing direction should be updated by client
	this->currentGameInstance->updatePosition(this->player.playerIndex, doc["player"]["position"][0].GetFloat(), doc["player"]["position"][1].GetFloat(), 0, 0, FacingDirection::UP);
	//this->player.updatePosition();

	//std::cout << "Player " << this->player.userID << ": " << this->player.position.first << ", " << this->player.position.second << "\n";
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

	this->currentGameInstance->attackTarget(this->player.playerIndex, player_x, player_y, hitbox_x, hitbox_y, hitbox_rotation);
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

		/**
		* Begin loop through mobs
		*/
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
		/**
		* End loop through mobs
		*/

		data.AddMember("action", rapidjson::Value("INIT"), root.GetAllocator());
		root.AddMember("data", data, root.GetAllocator());

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		root.Accept(writer);
		return std::string(buffer.GetString());

	}
	return "";
}

/**
 * Returns:
 * std::string or "" (empty string)
 */
std::string GameClient::generatePositionUpdate() {
	if (this->currentGameInstance != nullptr) {
		std::vector<GamePlayer*> players = this->currentGameInstance->getPlayerPositions();
		size_t f = players.size();

		if (players[0] != nullptr) {
			rapidjson::Document root;
			root.SetObject();

			root.AddMember("status", "OK", root.GetAllocator());

			rapidjson::Value data;
			data.SetObject();

			rapidjson::Value update_value("UPDATE");
			data.AddMember("action", update_value, root.GetAllocator());


			rapidjson::Value players_array;
			players_array.SetArray();

			/*
			 * Begin loop through players
			 */
			rapidjson::Value player;
			player.SetObject();
			//player_value.SetObject();
			//player_value.AddMember("x", rapidjson::Value(players[0]->position.first), root.GetAllocator());
			//player_value.AddMember("y", rapidjson::Value(players[0]->position.second), root.GetAllocator());

			rapidjson::Value position;
			position.SetArray();
			position.PushBack(players[0]->positionX, root.GetAllocator());
			position.PushBack(players[0]->positionY, root.GetAllocator());
			player.AddMember("position", position, root.GetAllocator());

			rapidjson::Value velocity;
			velocity.SetArray();
			velocity.PushBack(players[0]->velocityX, root.GetAllocator());
			velocity.PushBack(players[0]->velocityY, root.GetAllocator());
			player.AddMember("velocity", velocity, root.GetAllocator());

			players_array.PushBack(player, root.GetAllocator());
			/*
			 * End Loop through players
			 */
			data.AddMember("players", players_array, root.GetAllocator());

			/*
			* Hitbox debugging (if exists)
			*/
			rapidjson::Value hitboxes;
			hitboxes.SetArray();

			auto boxes = this->currentGameInstance->hitboxes;
			for (auto hitbox : boxes) {
				rapidjson::Value json_hitbox;
				json_hitbox.SetArray();
				for (auto point : hitbox) {
					rapidjson::Value json_box;
					json_box.SetArray();
					json_box.PushBack(point[0], root.GetAllocator());
					json_box.PushBack(point[1], root.GetAllocator());
					json_hitbox.PushBack(json_box, root.GetAllocator());
				}
				hitboxes.PushBack(json_hitbox, root.GetAllocator());
			}
			data.AddMember("hitboxes", hitboxes, root.GetAllocator());

			this->currentGameInstance->hitboxes.clear();
			/*
			 * End Hitbox debugging
			 /

			/**
			 * Begin loop through mobs
			 */
			rapidjson::Value mob_array;
			mob_array.SetArray();
			for (GameMob* game_mob : this->currentGameInstance->mobs) {
				rapidjson::Value mob;
				mob.SetObject();

				rapidjson::Value position;
				position.SetArray();
				position.PushBack(game_mob->positionX, root.GetAllocator());
				position.PushBack(game_mob->positionY, root.GetAllocator());

				mob.AddMember("token", rapidjson::Value(game_mob->token, strlen(game_mob->token)), root.GetAllocator());
				mob.AddMember("position", position, root.GetAllocator());
				mob_array.PushBack(mob, root.GetAllocator());
			}
			data.AddMember("mobs", mob_array, root.GetAllocator());
			/**
			 * End loop through mobs
			 */

			root.AddMember("data", data, root.GetAllocator());

			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			root.Accept(writer);
			return std::string(buffer.GetString());
		}
	}

	return "";
}