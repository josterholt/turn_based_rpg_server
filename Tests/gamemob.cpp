#include "stdafx.h"
#include "CppUnitTest.h"
#include "utils.h"
#include "gamemob.h"
#include "gameunit.h"
#include "mobs/goblin.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			GameUnit mob;
			Assert::IsTrue(mob.canMove == true);
		}

		TEST_METHOD(TestMethod2)
		{
			GameUnit mob("Test Mob", 15, 15, 32, 32);
		}

		TEST_METHOD(GoblinTest)
		{
			Goblin goblin("Test Goblin", 15, 15, 32, 32);
		}
	};
}