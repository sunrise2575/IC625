#include "common.h"

void hw::object_t::__make_vbo(hw::shape_t& shape) {
	glBindVertexArray(this->VAO);
		glGenBuffers(1, &shape.vbo_idx);
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_idx);
		glBufferData(GL_ARRAY_BUFFER,
				3 * sizeof(GLfloat) * shape.arr.size(), shape.arr.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void hw::object_t::__remove_vbo(const GLuint vbo_idx) {
	glBindVertexArray(this->VAO);
		glDeleteBuffers(1, &vbo_idx); 
	glBindVertexArray(0);
}

void hw::object_t::__place(const glm::vec3& source) {
	this->position = std::move(source);
	this->mat_trans = glm::translate(glm::mat4(1.0f), this->position); 
}

hw::object_t::object_t() {
	this->mat_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	this->mat_rot = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	this->mat_scale = glm::scale(glm::mat4(1.0f), glm::vec3((float)hw::GRID_WIDTH / 2.0f));
}

hw::object_t::~object_t() {
	this->__remove_vbo(this->vertex.vbo_idx);
	this->__remove_vbo(this->color.vbo_idx);
}

void hw::object_t::set_VAO(const GLuint VAO) {
	this->VAO = VAO;
}

void hw::object_t::set_shader(const GLuint shader_pid) {
	this->SHADER = shader_pid;
}

void hw::object_t::bind_vbo_to_vao() {
	glBindVertexArray(this->VAO);
		GLint vertex_attr = glGetAttribLocation(this->SHADER, "vertex_attr");
		if (vertex_attr == -1) { fprintf(stderr, "Shader failure: vertex_attr\n"); exit(EXIT_FAILURE); }
		glBindBuffer(GL_ARRAY_BUFFER, this->vertex.vbo_idx);
		glEnableVertexAttribArray(vertex_attr);
		glVertexAttribPointer(vertex_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);

		GLint color_attr = glGetAttribLocation(this->SHADER, "color_attr");
		if (color_attr == -1) { fprintf(stderr, "Shader failure: color_attr\n"); exit(EXIT_FAILURE); }
		glBindBuffer(GL_ARRAY_BUFFER, this->color.vbo_idx);
		glEnableVertexAttribArray(color_attr);
		glVertexAttribPointer(color_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
}

void hw::object_t::draw() {
	GLuint transform_mat = glGetUniformLocation(this->SHADER, "transform_mat");
	glUniformMatrix4fv(transform_mat, 1, GL_FALSE, glm::value_ptr(this->get_mat()));
	glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, this->vertex.arr.size());
	glBindVertexArray(0);
}

void hw::object_t::set_vertex(const hw::shape_t& shape) {
	this->vertex = std::move(shape);
	this->__make_vbo(this->vertex);
}

void hw::object_t::set_color(const hw::shape_t& shape) {
	this->color = std::move(shape);
	this->__make_vbo(this->color);
}

void hw::object_t::place(const GLint x, const GLint y) {
	this->position = glm::vec3(x, y, this->position.z); 
	this->mat_trans = glm::translate(glm::mat4(1.0f), this->position); 
}

void hw::object_t::place_on_grid(const GLint grid_x, const GLint grid_y) {
	glm::vec3 a{
			(float)grid_x * (float)hw::GRID_WIDTH,
			(float)hw::CAMERA_HEIGHT / 2.0f - (float)hw::GRID_WIDTH / 2.0f - (float)grid_y * (float)hw::GRID_WIDTH,
			0.0f};
	this->__place(a);
}

void hw::object_t::scale(const GLfloat x, const GLfloat y) {
	this->mat_scale *= glm::scale(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
}

glm::mat4 hw::object_t::get_mat() { return this->mat_trans * this->mat_rot * this->mat_scale; }

glm::mat4 hw::player_t::get_camera() {
	return this->mat_proj * this->mat_view * this->mat_model;
}
void hw::player_t::set_mat_proj(glm::mat4 mat) { this->mat_proj = mat; }
void hw::player_t::set_mat_view(glm::mat4 mat) { this->mat_view = mat; }
void hw::player_t::set_mat_model(glm::mat4 mat) { this->mat_model = mat; }

void hw::player_t::set_view(const glm::vec3 at, const glm::vec3 to, const glm::vec3 up) {
	this->view_at = at; this->view_to = to; this->view_up = up;
	this->mat_view = glm::lookAt(this->view_at, this->view_to, this->view_up);
}

void hw::player_t::move_camera(const GLfloat dx, const GLfloat dy) {
	this->view_at += glm::vec3(dx, dy, 1);
	this->mat_view = glm::lookAt(this->view_at, glm::vec3(0, 0, -1) + this->view_at, this->view_up);
}

void hw::player_t::place_on_grid(const GLint grid_x, const GLint grid_y) {
	this->grid_x = grid_x;
	this->grid_y = grid_y;

	glm::vec3 a{
			(float)grid_x * (float)hw::GRID_WIDTH,
			(float)hw::CAMERA_HEIGHT / 2.0f - (float)hw::GRID_WIDTH / 2.0f - (float)grid_y * (float)hw::GRID_WIDTH,
			0.0f};
	this->__place(a);
}

void hw::player_t::move_on_grid(const GLint grid_dx, const GLint grid_dy) {
	this->grid_x += grid_dx;
	this->grid_y += grid_dy;
	this->place_on_grid(this->grid_x, this->grid_y);
}

glm::vec2 hw::player_t::get_grid() {
	return glm::vec2(this->grid_x, this->grid_y);
}

void hw::player_t::key_input(int key, int action) {
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		if (this->get_grid().y == 0) { return; }
		if ((*this->map_iter)->type() == hw::tile_line_type::ground) {
			for (auto& e : static_cast<hw::ground_t*>(*this->map_iter)->elements) {
				if (e.get_grid().y == this->grid_y - 1) {
					return;
				}
			}
		}

		// success
		this->move_on_grid(0, -1);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		if (this->get_grid().y == hw::GRIDS - 1) { return; }
		if ((*this->map_iter)->type() == hw::tile_line_type::ground) {
			for (auto& e : static_cast<hw::ground_t*>(*this->map_iter)->elements) {
				if (e.get_grid().y == this->grid_y + 1) {
					return;
				}
			}
		}

		// success
		this->move_on_grid(0, 1);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		std::list<hw::tile_line_t*>::iterator p = std::prev(this->map_iter, 1);
		if (this->map_iter == hw::map.begin()) { return; }

		// blocked 
		if ((*p)->type() == hw::tile_line_type::ground) {
			for (auto& e : static_cast<hw::ground_t*>(*p)->elements) {
				if (e.get_grid().y == this->grid_y) {
					return;
				}
			}
		}

		// success
		if (this->position.x > this->view_at.x - 9.0f * (float)hw::GRID_WIDTH) {
			this->move_on_grid(-1, 0);
			std::advance(this->map_iter, -1);
		}
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		std::list<hw::tile_line_t*>::iterator p = std::next(this->map_iter, 1);
		if (p == hw::map.end()) { return; }
		
		// blocked 
		if ((*p)->type() == hw::tile_line_type::ground) {
			for (auto& e : static_cast<hw::ground_t*>(*p)->elements) {
				if (e.get_grid().y == this->grid_y) {
					return;
				}
			}
		}

		// success
		this->move_on_grid(1, 0);
		std::advance(this->map_iter, 1);
		if (this->position.x > this->view_at.x - 6.0f * (float)hw::GRID_WIDTH) {
			this->move_camera((float)hw::GRID_WIDTH, 0);
			// create
			/*
			int next_idx = (*std::prev(hw::map.end(), 1))->get_index() + 1;
			if (hw::random::binary(0.4)) {
				hw::map.push_back(new hw::ground_t(next_idx));
			} else {
				hw::map.push_back(new hw::road_t(next_idx));
			}
			*/
		}
	}
}

void hw::player_t::check_dead() {
	if (this->map_iter == hw::map.begin()) { return; }

	// blocked 
	if ((*this->map_iter)->type() == hw::tile_line_type::road) {
		for (auto& e : static_cast<hw::road_t*>(*this->map_iter)->elements) {
			if (e.get_pos().y - ((float)hw::GRID_WIDTH * 0.7f) < this->position.y
					&& e.get_pos().y + ((float)hw::GRID_WIDTH * 0.7f) > this->position.y) {
				this->map_iter = hw::map.begin();
				this->place_on_grid(0, hw::GRIDS/2);
				this->view_at = glm::vec3(8.0f * (float)hw::GRID_WIDTH, 0, 1);
				this->mat_view = glm::lookAt(this->view_at, glm::vec3(0, 0, -1) + this->view_at, this->view_up);
				return;
			}
		}
	}
	
}

void hw::player_t::set_starting_line() {
	this->map_iter = hw::map.begin();
}

void hw::tree_t::place_on_grid(const GLint grid_x, const GLint grid_y) {
	this->grid_x = grid_x;
	this->grid_y = grid_y;

	glm::vec3 a{
			(float)grid_x * (float)hw::GRID_WIDTH,
			(float)hw::CAMERA_HEIGHT / 2.0f - (float)hw::GRID_WIDTH / 2.0f - (float)grid_y * (float)hw::GRID_WIDTH,
			0.0f};
	this->__place(a);
}

glm::vec2 hw::tree_t::get_grid() {
	return glm::vec2(this->grid_x, this->grid_y);
}

void hw::car_t::set_speed(const GLfloat speed) {
	this->speed = speed;
}

glm::vec3 hw::car_t::get_pos() {
	return this->position;
}

void hw::car_t::move() {
	this->position.y += this->speed;
	this->mat_trans *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, this->speed, 0.0f));
}
