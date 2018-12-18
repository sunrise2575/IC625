#include <random>
#include "map.h"

static int random_uniform_int(const int min, const int max) {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<int> d(min, max);
	return d(gen);
}

static float random_uniform_float(const float min, const float max) {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<float> d(min, max);
	return d(gen);
}

static bool random_binary(const float probability) {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::bernoulli_distribution d(probability);
	return d(gen);
}

float map_t::grid_x(int x) {
	return GRID_SIZE * x;
}

float map_t::grid_y(int y) {
	return GRID_SIZE * y;
}

void map_t::find_major_and_minor_max() {
	// what is player?
	for (auto i = this->assets->begin(); i != this->assets->end(); ++i) {
		if (i->second.major_name == std::string(PLAYER_NAME)) {
			this->player_major = i->first.major_type;
			break;
		}
	}

	// what is ground?
	for (auto i = this->assets->begin(); i != this->assets->end(); ++i) {
		if (i->second.major_name == std::string(GROUND_NAME)) {
			this->ground_major = i->first.major_type;
			break;
		}
	}

	for (auto i = this->assets->begin(); i != this->assets->end(); ++i) {
		if (i->second.major_name == std::string(ROAD_NAME)) {
			this->road_major = i->first.major_type;
			break;
		}
	}

	// what is obstacles?
	for (auto i = this->assets->begin(); i != this->assets->end(); ++i) {
		if (i->second.major_name == std::string(OBSTACLES_NAME)) {
			this->obstacle_major = i->first.major_type;
			break;
		}
	}

	// what is vehicle?
	for (auto i = this->assets->begin(); i != this->assets->end(); ++i) {
		if (i->second.major_name == std::string(VEHICLES_NAME)) {
			this->vehicle_major = i->first.major_type;
			break;
		}
	}

	// find the maximum of minimum number
	for (auto i = this->assets->begin(); i != this->assets->end(); ++i) {
		if (i->first.major_type == this->obstacle_major
			&& i->first.minor_type > this->obstacle_minor_max)
		{
			this->obstacle_minor_max = i->first.minor_type;
		}
		if (i->first.major_type == this->vehicle_major
			&& i->first.minor_type > this->vehicle_minor_max)
		{
			this->vehicle_minor_max = i->first.minor_type;
		}
	}
}

void map_t::make_player() {
	this->player.asset(asset_key_t{ this->player_major, 1 });
	this->player.create_humanoid();
	//this->player.v_trans_rel(0, 0, GRID_SIZE);
	//this->player.v_scale(0.8f, 0.8f, 0.8f);
	this->player_return_to_start();
}

void map_t::player_return_to_start() {
	this->player.v_trans(this->grid_x(this->x / 2), this->grid_y(0), this->player.v_trans().z);
	this->player_x = this->x / 2;
	this->player_y = 0;
}

void map_t::make_obstacles() {
	for (unsigned int i = 0; i < this->y; i++) { 
		if (this->ground_or_road[i] && i != 0) {
			// if it is ground
			for (unsigned int j = 0; j < this->x; j++) {
				if (random_binary(OBSTACLE_PROB)) {
					unsigned int minor = random_uniform_int(1, this->obstacle_minor_max);
					instance_t tmp;
					tmp.asset(asset_key_t{ this->obstacle_major, minor });
					this->obstacle_map[i][j] = true;
					float x = this->grid_x(j);
					float y = this->grid_y(i);
					tmp.v_trans(x, y, 0);

					this->obstacles[i].push_back(tmp);
				}
			}
		}
	}
}

void map_t::make_road() {
	for (unsigned int i = 0; i < this->y; i++) {
		// if it is road
		if (!this->ground_or_road[i]) {
			this->road_info[i].asset_key.major_type = this->vehicle_major;
			this->road_info[i].asset_key.minor_type = random_uniform_int(1, this->vehicle_minor_max);
			this->road_info[i].is_direction_positive_x = random_binary(VEHICLE_DIRECTION_PROB);
			this->road_info[i].speed = random_uniform_float(3.0f, 7.0f);
		}
	}
}

map_t::map_t() {
	this->player_major = 0;
	this->player_x = 0;
	this->player_y = 0;
	std::memset(&this->kstate, GLFW_RELEASE, sizeof(this->kstate));
	
	// obstacles
	this->road_major = 0;
	this->ground_major = 0;
	this->obstacle_major = 0;
	this->obstacle_minor_max = 0;
	this->vehicle_major = 0;
	this->vehicle_minor_max = 0;
}

void map_t::bind_assets(asset_kv_t *assets) {
	this->assets = assets;
}

void map_t::generate(const int x, const int y) {
	this->x = x;
	this->y = y;

	// Allocate obstacle map
	this->obstacle_map.resize(y);
	for (auto& i : this->obstacle_map) {
		i.resize(x);
	}

	// Allocate ground objects
	this->grounds.resize(y);
	for (auto& i : this->grounds) {
		i.resize(x);
	}

	this->ground_or_road.resize(y);
	this->road_info.resize(y);
	this->obstacles.resize(y);
	this->vehicles.resize(y);

	this->find_major_and_minor_max();

	// decide ground_or_road
	unsigned int size = (unsigned int)ground_or_road.size();
	for (unsigned int i = 0; i < size; i++) { 
		this->ground_or_road[i] = i ? random_binary(GROUND_PROB) : true;
	}

	// place ground object
	for (int j = 0; j < (int)this->y; j++) {
		for (int i = 0; i < (int)this->x; i++) {
			if (this->ground_or_road[j]) {
				this->grounds[j][i].asset(asset_key_t{ this->ground_major, 1 });
			}
			else {
				this->grounds[j][i].asset(asset_key_t{ this->road_major, 1 });
			}
			this->grounds[j][i].v_scale(GRID_SIZE, 0.01f, GRID_SIZE);
			this->grounds[j][i].v_trans((float)i*GRID_SIZE, (float)j*GRID_SIZE, -0.02f);
		}
	}

	this->make_player();
	this->make_obstacles();
	this->make_road();
}

void map_t::update_vehicle() {
	for (unsigned int i = 0; i < this->y; i++) { 
		if (!this->ground_or_road[i]) {
			// found car list!
			auto& vl = this->vehicles[i];

			float speed = this->road_info[i].speed
				* ((this->road_info[i].is_direction_positive_x) ? 1.0f : -1.0f);

			// add car!
			if (random_binary(VEHICLE_PROB)) {
				instance_t temp;
				if (this->road_info[i].is_direction_positive_x) {
					temp.v_trans(this->grid_x(-1), this->grid_y(i), 0.0);
				}
				else {
					temp.v_trans(this->grid_x(this->x), this->grid_y(i), 0.0);
					temp.rot_z_axis(180);
				}
				temp.asset(this->road_info[i].asset_key);
				vl.push_back(temp);
			}
			
			// update movement!
			for (auto& j : vl) {
				j.v_trans_vel(speed, 0, 0);
			}
		}
	}

	// remove outbound vehicle!
	for (unsigned int i = 0; i < this->y; i++) { 
		if (!this->ground_or_road[i]) {
			while (this->vehicles[i].size() > 0) {
				auto pos = this->vehicles[i].front().v_trans().x;
				if (pos > this->grid_x(this->x + 1) || pos < this->grid_x(-2)) {
					this->vehicles[i].pop_front();
				}
				else {
					break;
				}
			}
		}
	}
}

bool map_t::is_player_dead(instance_t& opponent) {
	asset_value_t& other = this->assets->find(opponent.asset())->second;
	asset_value_t& player = this->assets->find(this->player.asset())->second;
	
	float player_scale = this->player.v_scale().z;
	float other_scale = opponent.v_scale().z;

	float player_pos = this->player.v_trans().x;
	float other_pos = opponent.v_trans().x;

	float player_lower = player_pos + player.z_min * player_scale;
	float player_upper = player_pos + player.z_max * player_scale;
	float other_lower = other_pos + other.z_min * other_scale;
	float other_upper = other_pos + other.z_max * other_scale;

	if (other_upper < player_lower) {
		return false;
	}
	if (other_lower > player_upper) {
		return false;
	}
	return true;
}

void map_t::draw(const shader_id_t& shader_id) {

	for (auto& i : this->grounds) {
		for (auto& j : i) {
			j.draw(*this->assets, shader_id);
		}
	}

	for (auto& i : this->obstacles) {
		for (auto& j : i) {
			j.draw(*this->assets, shader_id);
		}
	}

	this->player.draw(*this->assets, shader_id);

	for (auto& i : this->vehicles) {
		for (auto& j : i) {
			j.draw(*this->assets, shader_id);
		}
	}

}

void map_t::player_keyinput_move(GLFWwindow *&window) {
	// W Key (front)
	this->kstate.w_new = glfwGetKey(window, GLFW_KEY_W);
	if (this->kstate.w_new == GLFW_PRESS && this->kstate.w_old == GLFW_RELEASE) {
		// collision check
		if (this->player_y < (int)this->y - 1) {
			if (!this->obstacle_map[this->player_y + 1][this->player_x]) {
				this->player.v_trans_rel(0, GRID_SIZE, 0);
				this->player_y++;
			}
		}
	}
	this->kstate.w_old = this->kstate.w_new;

	// S Key (back)
	this->kstate.s_new = glfwGetKey(window, GLFW_KEY_S);
	if (this->kstate.s_new == GLFW_PRESS && this->kstate.s_old == GLFW_RELEASE) {
		// collision check
		if (this->player_y > 0) {
			if (!this->obstacle_map[this->player_y - 1][this->player_x]) {
				this->player.v_trans_rel(0, -GRID_SIZE, 0);
				this->player_y--;
			}
		}
	}
	this->kstate.s_old = this->kstate.s_new;

	// A Key (left)
	this->kstate.a_new = glfwGetKey(window, GLFW_KEY_A);
	if (this->kstate.a_new == GLFW_PRESS && this->kstate.a_old == GLFW_RELEASE) {
		// collision check
		if (this->player_x > 0) {
			if (!this->obstacle_map[this->player_y][this->player_x - 1]) {
				this->player.v_trans_rel(-GRID_SIZE, 0, 0);
				this->player_x--;
			}
		}
	}
	this->kstate.a_old = this->kstate.a_new;
	
	// D Key (right)
	this->kstate.d_new = glfwGetKey(window, GLFW_KEY_D);
	if (this->kstate.d_new == GLFW_PRESS && this->kstate.d_old == GLFW_RELEASE) {
		// collision check
		if (this->player_x < (int)this->x - 1) {
			if (!this->obstacle_map[this->player_y][this->player_x + 1]) {
				this->player.v_trans_rel(GRID_SIZE, 0, 0);
				this->player_x++;
			}
		}
	}
	this->kstate.d_old = this->kstate.d_new;
}

void map_t::dead_check() {
	for (auto& i : this->vehicles[this->player_y]) {
		if (this->is_player_dead(i)) {
			printf("You're dead! Max score: %d\n", this->player_y);
			this->player_return_to_start();
		}
	}
}