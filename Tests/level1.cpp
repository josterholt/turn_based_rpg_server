#include "stdafx.h"
#include "CppUnitTest.h"
#include "gamestate.h"
#include "gameclient.h"
#include <fstream>
#include "rapidjson/istreamwrapper.h"
#include <iostream>
#include "utils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(LevelJSON)
	{
	public:

		TEST_METHOD(NewGameState)
		{
			std::string level = "level1";
			std::cout << "Loading " << level << ".json\n";
			std::string filename = "C:/Users/Justin/MiscCode/Game/combat_system/src/assets/tiled_map/" + level + ".json";
			std::ifstream file(filename.c_str());

			if ((file.rdstate() & std::ifstream::failbit) != 0) {
				Assert::Fail(L"Unable to open file");
			}

			rapidjson::Document doc;
			rapidjson::IStreamWrapper isw(file);
			doc.ParseStream(isw);
			file.close();

			std::cout << doc.GetParseError() << "\n";

			std::string decoded_string;
			rapidjson::Value tiles = doc["layers"][0]["data"].GetArray();
			//for (rapidjson::Value::ValueIterator it = tiles.Begin(); it != tiles.End(); ++it) {
				//this->tiles.push_back(it->GetInt());
			//}

			/*
			//decoded_string = std::string(doc["layers"][0]["data"].GetString());
			for (int i = 0; i < tiles.size(); i = i + 4) {
			this->tiles.push_back((unsigned char) decoded_string[i]);
			}
			*/
			Assert::IsTrue(doc["layers"].Size() > 0, L"Map has more than one layer");
			rapidjson::Value object_layer = doc["layers"][1].GetObject();
			Assert::IsFalse(object_layer.ObjectEmpty());
			Assert::IsTrue(object_layer["objects"].IsArray(), L"objects key is an array");
			Assert::IsTrue(object_layer["objects"].Size() > 0, L"Object layer is array");

			rapidjson::Value objects = object_layer["objects"].GetArray();
			Assert::IsFalse(objects.Empty(), L"objects.Empty() is False");
			Assert::IsTrue(objects.Size() > 0, L"objects.size() > 0");
			Assert::IsTrue(objects[0].IsObject(), L"object layer[0] is object");
			Assert::IsTrue(objects[0]["x"].IsInt(), L"x is int");
			//float x = objects[0]["x"].GetFloat();

			Assert::IsTrue(objects[0]["y"].IsInt(), L"x is int");
			//float y = objects[0]["y"].GetFloat();
			
			//std::pair<float, float> position = std::make_pair(doc["layers"][1]["objects"][0]["x"].GetInt(), doc["layers"][1]["objects"][0]["y"].GetInt());
			std::pair<float, float> position = std::make_pair(objects[0]["x"].GetFloat(), objects[0]["y"].GetFloat());
		}
	};
}