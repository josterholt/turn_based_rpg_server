#include "stdafx.h"
#include "CppUnitTest.h"
#include "gamemanager.h"
#include "utils.h"
#include "protobuf\connect.pb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(GameManagerTests)
	{
	public:

		TEST_METHOD(ConnectRequestNoToken)
		{
			// Connect message with empty token
			gamemessages::Connect connect;
			Assert::IsTrue(connect.gametoken() == "");

			GameManager& game_manager = GameManager::getInstance();
			Assert::IsTrue(game_manager.getGameCount() == 0);

			const std::string& game_token = game_manager.handleGameRequest(connect.gametoken());
			GameState* game_state = game_manager.getGame(game_token);
			Assert::IsTrue(GameManager::getGameCount() == 1);
			Assert::AreEqual(game_token, game_state->getToken());

			//GameClient *client = new GameClient();


			// Assert token is null when none exists

			// Create instance of GameClient and token (null)

			// Assert new GameClient is 
			// Assert new game token is generated
			// Assert player index 0 is set 

			//delete client;
		}

		TEST_METHOD(ConnectRequestWithToken)
		{

		}

		TEST_METHOD(ConnectRequestNoPayload)
		{

		}
	};
}