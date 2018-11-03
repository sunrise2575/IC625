#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "map.h"
#include "global_var.h"
#include "camera.h"

camera_t::camera_t() {
	this->h_angle = PI / 2.0f;
	this->v_angle = 0.0f;
	this->mouse_speed = MOUSE_SPEED;
	this->move_speed = PLAYER_SPEED;
	this->inst = nullptr;
	this->pos_from_inst = glm::vec3(0.0);
}

void camera_t::bind_map(map_t *map) {
	this->map = map;
	this->inst = &this->map->player;
}

void camera_t::set_pos_from_inst(const glm::vec3 &vec) {
	this->pos_from_inst = std::move(vec);
}

void camera_t::proj_ortho() {
	this->proj = glm::ortho(
		-(float)WINDOW_WIDTH / 96.0f, (float)WINDOW_WIDTH / 96.0f,
		-(float)WINDOW_HEIGHT / 96.0f, (float)WINDOW_HEIGHT / 96.0f,
		0.1f, 100.0f);
}

void camera_t::proj_persp_first() {
	this->proj = glm::perspective(
		glm::radians(45.0f), 
		float(WINDOW_WIDTH) / float(WINDOW_HEIGHT),
		0.1f, 800.0f);
}

void camera_t::proj_persp_third() {
}

void camera_t::look_player() {
	/*
	glm::vec3 cam_target = this->inst->get_pos();
	glm::vec3 cam_direction = glm::normalize(this->pos - cam_target);
	glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cam_right = glm::normalize(glm::cross(world_up, cam_direction));
	glm::vec3 cam_up = glm::cross(cam_direction, cam_right);
	this->view = glm::lookAt(cam_target, cam_direction, cam_up);
	*/
	float cam_x = ((float)this->map->x * GRID_SIZE) / 2.0f;
	glm::vec3 target(cam_x, this->inst->get_pos().y, 0.0);
	this->pos = target + this->pos_from_inst;
	this->view = glm::lookAt(this->pos, target, glm::vec3(0.0, 0.0, 1.0));
}

void camera_t::look_anywhere_mouse(GLFWwindow *&window) {
	glfwGetCursorPos(window, &this->mouse_x, &this->mouse_y);
	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	// Compute new orientation
	this->h_angle += mouse_speed * float(float(WINDOW_WIDTH) / 2.0f - this->mouse_x);
	this->v_angle += mouse_speed * float(float(WINDOW_HEIGHT) / 2.0f - this->mouse_y);

	glm::vec3 direction(
		cos(this->v_angle) * cos(this->h_angle),
		cos(this->v_angle) * sin(this->h_angle),
		sin(this->v_angle)
	);

	glm::vec3 right(
		cos(this->h_angle - PI / 2.0f),
		sin(this->h_angle - PI / 2.0f),
		0.0f
	);

	glm::vec3 up = glm::cross(right, direction);
	this->view = glm::lookAt(this->pos, this->pos + direction, up);
}

void camera_t::keyboard_moving(GLFWwindow *&window) {
	/*
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		this->pos += this->direction * DELTA_T * this->move_speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		this->pos -= this->direction * DELTA_T * this->move_speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		this->pos += this->right * DELTA_T * this->move_speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		this->pos -= this->right * DELTA_T * this->move_speed;
	}
	*/
}

void camera_t::place_camera(const shader_id_t &shader_id)
{
	this->model = glm::mat4(1.0);

	glUniformMatrix4fv(shader_id.proj_mat, 1, GL_FALSE, &this->proj[0][0]);
	glUniformMatrix4fv(shader_id.view_mat, 1, GL_FALSE, &this->view[0][0]);
	glUniformMatrix4fv(shader_id.model_mat, 1, GL_FALSE, &this->model[0][0]);
	
}
