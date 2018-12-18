#ifndef __IC625_MAP_H__
#define __IC625_MAP_H__

#include <vector>
#include <deque>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "global_var.h"
#include "instance.h"
#include "scene_graph.h"

class camera_t;
class light_t;

struct road_info_t {
	asset_key_t asset_key;
	bool is_direction_positive_x;
	float speed;
};

struct key_state_t {
	int w_old, w_new;
	int a_old, a_new;
	int s_old, s_new;
	int d_old, d_new;
};

class map_t {
private:
	friend camera_t;
	friend light_t;
	// asset
	asset_kv_t* assets;

	// map_size
	unsigned int x, y;

	// player
	//instance_t player;
	scene_graph_t player;
	unsigned int player_major;
	int player_x, player_y;
	key_state_t kstate;
	
	// ground/road selector
	std::vector<bool> ground_or_road;

	unsigned int ground_major;
	unsigned int road_major;
	std::vector<std::vector<instance_t>> grounds;

	// obstacles
	unsigned int obstacle_major;
	unsigned int obstacle_minor_max;
	std::vector<std::deque<instance_t>> obstacles;
	std::vector<std::vector<bool>> obstacle_map;

	// road and vehicle
	std::vector<road_info_t> road_info;
	unsigned int vehicle_major;
	unsigned int vehicle_minor_max;
	std::vector<std::deque<instance_t>> vehicles;

	// internal function
	float grid_x(int x);
	float grid_y(int y);
	void make_obstacles();
	void make_road();
	void make_player();
	void find_major_and_minor_max();
	bool is_player_dead(instance_t& opponent);
	void player_return_to_start();
public:
	map_t();
	map_t(const map_t&) = delete;
	map_t(map_t&&) = delete;
	map_t& operator=(const map_t&) = delete;
	map_t& operator=(map_t&&) = delete;
	~map_t() = default;

	void bind_assets(asset_kv_t *assets);
	void generate(const int x, const int y);
	void player_keyinput_move(GLFWwindow *&window);
	void update_vehicle();
	void dead_check();
	void draw(const shader_id_t& shader_id);
};

#endif
