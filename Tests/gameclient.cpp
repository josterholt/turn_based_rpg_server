#include "stdafx.h"
#include "CppUnitTest.h"
#include "gameclient.h"
#include "utils.h"
#include "protobuf/connect.pb.h"
#include "protobuf/playerUpdate.pb.h"
#include <iostream>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class MockGameManager : public GameManager {

};

namespace Tests
{
	TEST_CLASS(ClientConnectTests)
	{
	public:
		TEST_METHOD_INITIALIZE(methodName)
		{
			GameManager::getInstance().destroyAllGames();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero a");
		}

		TEST_METHOD(ConnectNoToken)
		{
			gamemessages::Connect connect_message; // connect message with no token
			Assert::IsTrue(connect_message.gametoken() == "", L"Game token is empty");

			GameClient *client = new GameClient();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero b");

			Assert::IsTrue(client->handleConnectionMessage(connect_message), L"handleConnectionMessage");
			Assert::IsTrue(client->getGame()->getToken() != "", L"Game token is not empty");
			Assert::IsTrue(GameManager::getGameCount() == 1, L"Game count is 1");

			delete client;
		}

		/**
		 * Had trouble with an empty token, so needed to use NEW_GAME
		 */
		TEST_METHOD(ConnectNewGameToken)
		{
			gamemessages::Connect connect_message;
			connect_message.set_gametoken("NEW_GAME");
			Assert::IsTrue(connect_message.gametoken() == "NEW_GAME", L"Game token is empty");

			GameClient *client = new GameClient();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero b");

			Assert::IsTrue(client->handleConnectionMessage(connect_message), L"handleConnectionMessage");
			Assert::IsTrue(client->getGame()->getToken() != "", L"Game token is not empty");
			Assert::IsTrue(GameManager::getGameCount() == 1, L"Game count is 1");

			delete client;
		}

		TEST_METHOD(ConnectWithToken)
		{
			/**
			* SETUP REQUEST - CREATE FIRST GAME
			*/
			std::string my_str = "";
			const std::string& game_token = GameManager::getInstance().handleGameRequest(my_str); // @todo allow empty arguments
			gamemessages::Connect connect_message;
			connect_message.set_gametoken(game_token);
			Assert::IsTrue(connect_message.gametoken() == game_token, L"Game token is correct");


			/**
			* CREATE GAME
			*/
			GameClient *client = new GameClient();
			Assert::IsTrue(GameManager::getGameCount() == 1, L"Game count is 1"); // why wasn't this failing when games weren't destroyed?			


			/**
			* JOIN GAME WITH TOKEN
			*/
			Assert::IsTrue(client->handleConnectionMessage(connect_message), L"handleConnectionMessage");
			Assert::IsTrue(client->getGame()->getToken() == game_token, L"Game token matches");
			Assert::IsTrue(GameManager::getGameCount() == 1, L"Game count is 1");

			delete client;
		}

	};

	TEST_CLASS(ClientUpdatePlayerTests)
	{
	public:
		TEST_METHOD_INITIALIZE(methodName)
		{
			GameManager::getInstance().destroyAllGames();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero a");
		}

		TEST_METHOD(UpdatePlayerState)
		{
			gamemessages::Connect connect_message; // connect message with no token
			Assert::IsTrue(connect_message.gametoken() == "", L"Game token is empty");

			GameClient *client = new GameClient();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero b");

			/*
			 * Create game and then perform sanity check
			 */
			Assert::IsTrue(client->handleConnectionMessage(connect_message), L"handleConnectionMessage");
			Assert::IsTrue(client->getGame()->getToken() != "", L"Game token is not empty");
			Assert::IsTrue(GameManager::getGameCount() == 1, L"Game count is 1");

			float spawn_x = client->getGame()->spawnPoint.first;
			float spawn_y = client->getGame()->spawnPoint.second;

			gamemessages::PlayerUpdate update_message;
			gamemessages::Unit *unit = update_message.mutable_player();
			gamemessages::Point *position = unit->mutable_position();
			position->set_x(spawn_x + 4);
			position->set_y(spawn_y);

			gamemessages::Point *velocity = unit->mutable_velocity();
			velocity->set_x(0);
			velocity->set_y(0);
			
			client->updatePlayerState(update_message);

			std::vector<GamePlayer *> players = client->getGame()->getPlayerPositions();
			Assert::IsTrue(players[0]->positionX == spawn_x + 4, L"positionX equals 4");
			Assert::IsTrue(players[0]->positionY == spawn_y, L"positionY equals 0");
			Assert::IsTrue(players[0]->velocityX == 0, L"velocityX equals 0");
			Assert::IsTrue(players[0]->velocityY == 0, L"velocityY equals 0");


			delete client;
		}

		TEST_METHOD(InvalidPositionUpdate)
		{
			gamemessages::Connect connect_message; // connect message with no token
			Assert::IsTrue(connect_message.gametoken() == "", L"Game token is empty");

			GameClient *client = new GameClient();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero b");

			Assert::IsTrue(client->handleConnectionMessage(connect_message), L"handleConnectionMessage");
			Assert::IsTrue(client->getGame()->getToken() != "", L"Game token is not empty");
			Assert::IsTrue(GameManager::getGameCount() == 1, L"Game count is 1");

			float player_current_x = client->getGame()->getPlayerPositions()[0]->positionX;
			float player_current_y = client->getGame()->getPlayerPositions()[0]->positionY;

			/**
			 * Move player to position is isn't blocked but beyond max speed
			 */
			gamemessages::PlayerUpdate update_message;
			gamemessages::Unit *unit = update_message.mutable_player();
			gamemessages::Point *position = unit->mutable_position();
			position->set_x(player_current_x + 1000);
			position->set_y(player_current_y);

			gamemessages::Point *velocity = unit->mutable_velocity();
			velocity->set_x(64);
			velocity->set_y(0);

			client->updatePlayerState(update_message);

			std::vector<GamePlayer *> players = client->getGame()->getPlayerPositions();
			Assert::IsTrue(players[0]->positionX == player_current_x, L"positionX was not updated");
			Assert::IsTrue(players[0]->positionY == player_current_y, L"positionY was not updated");
			Assert::IsTrue(players[0]->velocityX == 0, L"velocityX is 0");
			Assert::IsTrue(players[0]->velocityY == 0, L"velocityY is 0");

			delete client;
		}
	};


	TEST_CLASS(ClientGetPlayerUpdateTests)
	{
	public:
		TEST_METHOD_INITIALIZE(methodName)
		{
			GameManager::getInstance().destroyAllGames();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero a");
		}

		TEST_METHOD(GetPlayerUpdateTests)
		{
			/**
			 * TEST SETUP
			 */
			gamemessages::Connect connect_message; // connect message with no token
			Assert::IsTrue(connect_message.gametoken() == "", L"Game token is empty");

			GameClient *client = new GameClient();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero b");

			Assert::IsTrue(client->handleConnectionMessage(connect_message), L"handleConnectionMessage");
			Assert::IsTrue(client->getGame()->getToken() != "", L"Game token is not empty");
			Assert::IsTrue(GameManager::getGameCount() == 1, L"Game count is 1");

			int spawn_x = 20;
			int spawn_y = 16;

			gamemessages::PlayerUpdate update_message;
			gamemessages::Unit *unit = update_message.mutable_player();
			gamemessages::Point *position = unit->mutable_position();
			position->set_x(spawn_x);
			position->set_y(spawn_y);
			client->updatePlayerState(update_message);
			/**
			 * END SETUP
			 */

			gamemessages::PositionUpdate position_update = client->generatePositionUpdate();
			Assert::IsTrue(spawn_x == position_update.players().Get(0).position().x(), L"update player position x assert");
			Assert::IsTrue(spawn_y == position_update.players().Get(0).position().y(), L"update player position y assert");
		}

	};
}
