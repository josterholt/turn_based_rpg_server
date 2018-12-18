#ifndef _SHAREDSTRUCTURES_H_
#define _SHAREDSTRUCTURES_H_
#include <vector> 

struct GameMap {
	GameMap() {}
	GameMap(int _map_width, int _map_height, int _tile_size, std::vector<unsigned int>* _tiles) : map_width(_map_width), map_height(_map_height), tile_size(_tile_size), tiles(_tiles) {}
	int map_width = 0;
	int map_height = 0;
	int tile_size = 0;
	std::vector<unsigned int>* tiles;
};
#endif