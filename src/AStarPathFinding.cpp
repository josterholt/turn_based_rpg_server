#include "AStarPathFinding.h"
#include <math.h>
#include <iostream>

std::vector<Vec2i> directions = {
	{1, 0},
	{0, 1},
	{-1, 0},
	{0, -1}
};

AStarPathFinding::AStarPathFinding(int _map_width, int _map_height, int _tile_size, std::vector<int>* _tiles) {
	this->_map_tile_width = _map_width;
	this->_map_tile_height = _map_height;
	this->_tile_size = _tile_size;

	for (int i = 0; i < _tiles->size(); ++i) {
		Point target = index_to_coords(this->_map_tile_width, this->_tile_size, this->_end_index);
		Point current_point = index_to_coords(this->_map_tile_width, this->_tile_size, i);
		Node* node = new Node(current_point.x, current_point.y, _tiles->at(i) != 0, this->get_distance(target.x, target.y, current_point.x, current_point.y), coords_to_index(this->_map_tile_width, this->_tile_size, current_point.x, current_point.y));
		this->_grid.push_back(node);
	}
}

/**
 * @todo Need to clean this up so it's not duplicated
 */
AStarPathFinding::AStarPathFinding(int _map_width, int _map_height, int _tile_size,  std::vector<unsigned int>* _tiles) {
	this->_map_tile_width = _map_width;
	this->_map_tile_height = _map_height;
	this->_tile_size = _tile_size;

	for (int i = 0; i < _tiles->size(); ++i) {
		Point current_point = index_to_coords(this->_map_tile_width, this->_tile_size, i);
		Node* node = new Node(current_point.x, current_point.y, _tiles->at(i) == 0, 0, coords_to_index(this->_map_tile_width, this->_tile_size, current_point.x, current_point.y));
		this->_grid.push_back(node);
	}
}

AStarPathFinding::~AStarPathFinding() {
	for(std::vector<Node *>::iterator it = this->_grid.begin(); it != this->_grid.end(); ++it) {
		delete *it;
	}
}

void AStarPathFinding::set_start_index(int i) {
	this->_start_index = i;
}

void AStarPathFinding::set_end_index(int i) {
	this->_end_index = i;

	Point target = index_to_coords(this->_map_tile_width, this->_tile_size, this->_end_index);

	for (std::vector<Node*>::iterator it = this->_grid.begin(); it != this->_grid.end(); ++it) {
		Point current_point = index_to_coords(this->_map_tile_width, this->_tile_size, (*it)->index);
		(*it)->distance = this->get_distance(target.x, target.y, current_point.x, current_point.y);
	}
}

Node* AStarPathFinding::get_node_by_index(int i) {
	if (i > this->_grid.size()) {
		return nullptr;
	}

	return this->_grid.at(i);
}

void AStarPathFinding::search() {
	Node* start_node = this->get_node_by_index(this->_start_index);
	if (start_node == nullptr) {
		return;
	}

	this->_open_list.insert(start_node);

	
	NodeList::iterator current_it;

	while(!this->_open_list.empty()) {
		current_it = this->_open_list.begin();
		_current = *current_it;

		if (this->_end_index == _current->index) {
			break;
		}

		this->_closed_list.push_back(_current->index);
		this->_open_list.erase(current_it);

		for (std::vector<Vec2i>::iterator dit = directions.begin(); dit != directions.end(); ++dit) {
			if (_current->x + (*dit).x >= this->_map_tile_width || _current->x + (*dit).x < 0) {
				continue;
			}

			if (_current->y + (*dit).y >= this->_map_tile_height || _current->y + (*dit).y < 0) {
				continue;
			}

			int index = coords_to_index(this->_map_tile_width, this->_tile_size, this->_current->x + (*dit).x * this->_tile_size, this->_current->y + (*dit).y * this->_tile_size);

			// Skip if node is in closed list
			std::vector<int>::iterator cit = std::find(this->_closed_list.begin(), this->_closed_list.end(), index);
			if (cit != this->_closed_list.end()) {
				continue;
			}

			// Update if on openset or new node
			Node *node;
			try {
				node = this->_grid.at(index);
			}
			catch (const std::exception& e) {
				std::cout << "Exception occurred";
				return;
			}

			if (node->blocked) {
				this->_closed_list.push_back(node->index);
				continue;
			}

			NodeList::iterator it = this->_open_list.find(node);

			int total_weight = _current->weight + 10; // Used to weed out most efficient path

			if (it == this->_open_list.end()) {
				node->parent = _current;
				node->weight = total_weight;
				this->_open_list.insert(node);
			}
			else if (total_weight < node->weight) { // Haven't found a reason conditional makes this more efficient yet
				node->parent = _current;
				node->weight = total_weight;
			}
		}
	}
}

std::vector<int> AStarPathFinding::get_path() {
	std::vector<int> path_indices;
	while (this->_current != nullptr) {
		path_indices.push_back(this->_current->index);
		this->_current = this->_current->parent;
	}
	return path_indices;
}

int coords_to_index(int tiles_per_row, int tile_size, int x, int y) {
	int index = floor(x / tile_size);
	index += (floor(y / tile_size) * tiles_per_row);
	return index;
	//return (y * tile_size) + (x);
}

Point index_to_coords(int tiles_per_row, int tile_size, int index) {
	Point p;
	p.y = tile_size * floor(index / tiles_per_row);
	p.x = tile_size * (index - (floor(index / tiles_per_row) * tiles_per_row));
	return p;
}

int AStarPathFinding::get_distance(int target_x, int target_y, int current_x, int current_y) {
	int distance = 0;
	if (target_x > current_x) {
		distance += target_x - current_x;
	}
	else {
		distance += current_x - target_x;
	}

	if (target_y > current_y) {
		distance += target_y - current_y;
	}
	else {
		distance += current_y - target_y;
	}
	return distance;
}