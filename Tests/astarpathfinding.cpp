#include "stdafx.h"
#include "CppUnitTest.h"
#include "AStarPathFinding.h"
#include <vector>
#include <iostream>
#include "gamestate.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(AStarPathFindingTests)
	{
	public:
		TEST_METHOD_INITIALIZE(methodName)
		{

		}

		TEST_METHOD(SimplePath)
		{
			std::vector<int> tiles = {
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 0, 1, 1, 1, 1, 1, 1, 0, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1
			};

			int start_index = 11;
			int end_index = 38;

			AStarPathFinding path_finding(10, 5, 1, &tiles);
			path_finding.set_start_index(start_index);
			path_finding.set_end_index(end_index);
			path_finding.search();
			std::vector<int> path = path_finding.get_path();

			Assert::AreEqual(end_index, path[0]);
			Assert::AreEqual(start_index, path[path.size() - 1]);
		}

		TEST_METHOD(LargePath)
		{
			GameState state;
			std::vector<unsigned int>* tiles = state.getTiles();

			int start_index = 101;
			int end_index = 504;

			AStarPathFinding path_finding(100, 100, 16, tiles);
			path_finding.set_start_index(start_index);
			path_finding.set_end_index(end_index);
			path_finding.search();

			std::vector<int> path = path_finding.get_path();
			Assert::IsTrue(path.size() > 1, L"Path has more than one element in it");
			Assert::AreEqual(end_index, path[0]);
			Assert::AreEqual(start_index, path[path.size() - 1]);
		};
	};
}
