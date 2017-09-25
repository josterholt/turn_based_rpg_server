#include "stdafx.h"
#include "CppUnitTest.h"
#include "gamestate.h"
#include "utils.h"

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

		TEST_METHOD(DetectsIntersection1)
		{
			xy_points_t points1 = { 
				{ 100, 200},
				{ 200, 200 },
				{ 200, 100 },
				{ 100, 100 }
			};

			xy_points_t points2 = { 
				{ 100, 250 },
				{ 290, 250 },
				{ 290, 150 },
				{ 100, 150 }
			};

			Assert::IsTrue(intersects(points1, points2), L"Points intersect");
		}

		TEST_METHOD(DetectsIntersection2)
		{
			xy_points_t points1 = {
				{ 100, 200 },
				{ 200, 200 },
				{ 200, 100 },
				{ 100, 100 }
			};

			xy_points_t points2 = {
				{ 190, 250 },
				{ 290, 250 },
				{ 290, 150 },
				{ 190, 150 }
			};

			Assert::IsFalse(intersects(points1, points2), L"Points do not intersect");
		}

		TEST_METHOD(PointRotation1)
		{
			point_t p = rotatePoint(1.0f, 0.0f, 90.0f);

			Assert::AreEqual(0.0f, p[0]);
			Assert::AreEqual(1.0f, p[1]);

			p = rotatePoint(1.0f, 0.0f, 180.0f);
			Assert::AreEqual(1.0f, p[0]);
			Assert::AreEqual(0.0f, p[1]);

		}

	};
}