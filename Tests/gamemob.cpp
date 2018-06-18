#include "stdafx.h"
#include "CppUnitTest.h"
#include "gamemob.h"
#include "utils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			GameMob mob;
			Assert::IsTrue(mob.canMove == true);
		}
	};
}