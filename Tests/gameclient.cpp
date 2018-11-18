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

	TEST_CLASS(PositionUpdateTests)
	{
	public:
		TEST_METHOD_INITIALIZE(methodName)
		{
			GameManager::getInstance().destroyAllGames();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero a");
		}

		TEST_METHOD(PositionUpdate)
		{
			gamemessages::Connect connect_message; // connect message with no token
			Assert::IsTrue(connect_message.gametoken() == "", L"Game token is empty");

			GameClient *client = new GameClient();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero b");

			Assert::IsTrue(client->handleConnectionMessage(connect_message), L"handleConnectionMessage");
			Assert::IsTrue(client->getGame()->getToken() != "", L"Game token is not empty");
			Assert::IsTrue(GameManager::getGameCount() == 1, L"Game count is 1");

			gamemessages::PlayerUpdate update_message;
			gamemessages::Unit *unit = update_message.mutable_player();
			gamemessages::Point *position = unit->mutable_position();
			position->set_x(20);
			position->set_y(16);

			gamemessages::Point *velocity = unit->mutable_velocity();
			velocity->set_x(4);
			velocity->set_y(0);
			
			client->updatePlayerState(update_message);

			delete client;
		}
	};
}