#include "common.h"
#define MIN_LANE_SPEED (float)hw::GRID_WIDTH / 16.0f
#define MAX_LANE_SPEED (float)hw::GRID_WIDTH / 5.0f
constexpr float DIRECTION_PROBABILITY = 0.5f;

hw::road_t::road_t(const int index) {
	this->index = index;
	this->is_plus_y_direction = hw::random::binary(DIRECTION_PROBABILITY);
	this->lane_speed = hw::random::uniform(MIN_LANE_SPEED, MAX_LANE_SPEED);
}

hw::ground_t::ground_t(const int index) { this->index = index; this->is_generated = false;}

int hw::road_t::get_index() { return this->index; }
int hw::ground_t::get_index() { return this->index; }

void hw::road_t::draw() { for (auto& t : this->elements) { t.draw(); } }
void hw::ground_t::draw() { for (auto& t : this->elements) { t.draw(); } }

hw::tile_line_type hw::road_t::type() { return hw::tile_line_type::road; }
hw::tile_line_type hw::ground_t::type() { return hw::tile_line_type::ground; }

void hw::road_t::gen(const GLuint shader_pid, const GLuint VAO) {
	if (hw::random::binary(this->lane_speed / (MAX_LANE_SPEED * 64.0f))) {
		this->elements.push_back(hw::car_t());

		hw::car_t& t = this->elements.back();
		t.set_vertex(hw::shape::car_vertex);
		t.set_color(hw::shape::car_color);
		t.set_VAO(VAO);
		t.set_shader(shader_pid);
		t.bind_vbo_to_vao();
		if (this->is_plus_y_direction) {
			t.place_on_grid(this->index, hw::GRIDS);
			t.set_speed(this->lane_speed);
		} else {
			t.place_on_grid(this->index, -1);
			t.set_speed(-this->lane_speed);
		}
		t.scale(0.8, 0.9);
	}
}

void hw::ground_t::gen(const GLuint shader_pid, const GLuint VAO) {
	if (this->is_generated) { return;}
	std::vector<bool> seq(hw::GRIDS);

	const int seq_sz = (int)seq.size();
	for (int i = 0; i < seq_sz; i++) {
		seq[i] = hw::random::binary(0.10);
	}

	for (int i = 0; i < seq_sz; i++) {
		if (seq[i]) {
			this->elements.push_back(hw::tree_t());

			hw::tree_t& t = this->elements.back();
			t.set_vertex(hw::shape::tree_vertex);
			t.set_color(hw::shape::tree_color);
			t.set_VAO(VAO);
			t.set_shader(shader_pid);
			t.place_on_grid(this->index, i);
			t.bind_vbo_to_vao();
		}
	}

	this->is_generated = true;

}


void hw::ground_t::move() {}
void hw::road_t::move() {
	for (auto& c : this->elements) {
		c.move();
	}
}

void hw::ground_t::remove() {}
void hw::road_t::remove() {
	for (auto i = this->elements.begin(); i != this->elements.end();) {
		if (i->get_pos().y < -(float)hw::CAMERA_HEIGHT / 2.0f - (float)hw::GRID_WIDTH ||
				i->get_pos().y > (float)hw::CAMERA_HEIGHT / 2.0f + (float)hw::GRID_WIDTH) {
			this->elements.erase(i);
		} else {
			++i;
		}
	}
}
