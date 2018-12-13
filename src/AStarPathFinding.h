#ifndef _ASTARPATHFINDING_H_
#define _ASTARPATHFINDING_H_
#include <vector>

struct Node {
	int index;
	int x;
	int y;
	int parent;
};

class AStarPathFinding {
	std::vector<int> open_list;
	std::vector<int> closed_list;
	std::vector<unsigned int> &tiles;
public:
	int start_index = 0;
	AStarPathFinding();
	void setTileList(std::vector<unsigned int> &tiles);
	void search();
};
#endif