#include "stdafx.h"
#include "CppUnitTest.h"
#include "gamestate.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			GameState state;
			Assert::AreEqual(state.gameID, 0);
		}

	};
}