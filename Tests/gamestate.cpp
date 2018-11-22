#include "stdafx.h"
#include "CppUnitTest.h"
#include "gameclient.h"
#include "gamestate.h"
#include "gameplayer.h";
#include "gamemanager.h"
#include "utils.h"
#include "protobuf/connect.pb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(GameStateTests)
	{
	public:
		TEST_METHOD_INITIALIZE(methodName)
		{
			GameManager::getInstance().destroyAllGames();
			Assert::IsTrue(GameManager::getGameCount() == 0, L"Game Count is zero a");
		}


		TEST_METHOD(NewGameState)
		{
			GameState game_state;


			GameClient game_client;
			//GamePlayer game_player(game_client); // @todo Decouple client from player

			gamemessages::Connect connect_message;
			game_client.handleConnectionMessage(connect_message);
			
			Assert::AreEqual(game_client.getGame()->players[0]->positionX, game_state.spawnPoint.first);
			Assert::AreEqual(game_client.getGame()->players[0]->positionY, game_state.spawnPoint.second);
		}
	};
}