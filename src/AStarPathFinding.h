#ifndef _ASTARPATHFINDING_H_
#define _ASTARPATHFINDING_H_
#include <vector>
#include <set>

struct Vec2i
{
	int x, y;
	bool operator == (const Vec2i& coordinates_)
	{
		return (x == coordinates_.x && y == coordinates_.y);
	}

	Vec2i operator+(const Vec2i& rhs)
	{
		return { x + rhs.x, y + rhs.y };
	}
};

struct Point {
	Point() {};
	Point(int x, int y) : x(x), y(y) {};
	int x;
	int y;
};

int coords_to_index(int tiles_per_row, int tile_size, int x, int y);
Point index_to_coords(int tiles_per_row, int tile_size, int index);

struct Node {
	Node(int _x, int _y, bool _blocked, int _distance, int _index) : x(_x), y(_y), blocked(_blocked), distance(_distance), index(_index) {}

	int index;
	int x;
	int y;
	Node* parent = nullptr;
	bool blocked;
	int distance;
	int weight;

	int getWeight() const {
		return distance + weight;
	}
};

struct NodeComp {
	bool operator() (const Node* lhs, const Node* rhs) const
	{
		return lhs->getWeight() < rhs->getWeight();
	}
};

typedef std::multiset<Node*, NodeComp> NodeList; // So we don't need to type out 

class AStarPathFinding {
	std::vector<Node *> _grid;
	NodeList _open_list;
	std::vector<int> _closed_list;

	int _map_tile_width = 0;
	int _map_tile_height = 0;
	int _tile_size = 0;

	int _start_index = 0;
	int _end_index = 0;
	Node* _current = nullptr;

	std::vector<int> blocked_nodes;

	Node* get_node_by_index(int i);
	int get_distance(int target_x, int target_y, int current_x, int current_y);
public:
	AStarPathFinding(int _map_tile_width, int _map_tile_height, int _tile_size, std::vector<int>* _tiles);
	AStarPathFinding(int _map_tile_width, int _map_tile_height, int _tile_size, std::vector<unsigned int>* _tiles);
	~AStarPathFinding();
	void set_start_index(int i);
	void set_end_index(int i);
	void search();
	std::vector<int> AStarPathFinding::get_path();
};
#endif