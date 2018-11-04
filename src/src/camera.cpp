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
	this->target = nullptr;
	this->model = glm::mat4(1.0);
	this->mode = 1;
	this->proj_ortho();
}

void camera_t::bind_map(map_t *map) {
	this->map = map;
	this->target = &this->map->player;
}

void camera_t::proj_ortho() {
	this->pos_from_target = glm::vec3(4.0, -6.0, 7.0);
	this->proj = glm::ortho(
		-(float)WINDOW_WIDTH / 96.0f, (float)WINDOW_WIDTH / 96.0f,
		-(float)WINDOW_HEIGHT / 96.0f, (float)WINDOW_HEIGHT / 96.0f,
		0.1f, 100.0f);
}

void camera_t::proj_persp_third() {
	this->pos_from_target = glm::vec3(0.0, -6.0, 7.0);
	this->proj = glm::perspective(
		glm::radians(70.0f), 
		float(WINDOW_WIDTH) / float(WINDOW_HEIGHT),
		0.1f, 800.0f);
}

void camera_t::proj_persp_first() {
	this->pos_from_target = glm::vec3(0.0, 0.0, 1.7);
	this->proj = glm::perspective(
		glm::radians(90.0f), 
		float(WINDOW_WIDTH) / float(WINDOW_HEIGHT),
		0.1f, 800.0f);
}

void camera_t::look_target(GLFWwindow *&window) {
	if (this->mode != 3) {
		float cam_x = (float(this->map->x - 1) / 2.0f * GRID_SIZE);
		glm::vec3 target(cam_x, this->target->get_pos().y, 0.0);
		this->pos = target + this->pos_from_target;
		this->view = glm::lookAt(this->pos, target, glm::vec3(0.0, 0.0, 1.0));
	}
	else {
		// first person mode
		this->pos = this->target->get_pos() + this->pos_from_target;

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
}

void camera_t::change_proj(GLFWwindow *&window) {
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		if (this->mode != 1) {
			this->mode = 1;
		}
		this->proj_ortho();
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		if (this->mode != 2) {
			this->mode = 2;
		}
		this->proj_persp_third();
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		if (this->mode != 3) {
			this->mode = 3;
		}
		this->proj_persp_first();
	}
}

void camera_t::place_camera(const shader_id_t &shader_id)
{
	glUniformMatrix4fv(shader_id.proj_mat, 1, GL_FALSE, &this->proj[0][0]);
	glUniformMatrix4fv(shader_id.view_mat, 1, GL_FALSE, &this->view[0][0]);
	glUniformMatrix4fv(shader_id.model_mat, 1, GL_FALSE, &this->model[0][0]);
}
