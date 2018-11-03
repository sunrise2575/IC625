#ifndef __IC625_CAMERA_H__
#define __IC625_CAMERA_H__

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include "instance.h"
#include "shader.h"

class map_t;

class camera_t {
private:
	// camera position, direction, right and up
	glm::vec3 pos; 
	glm::mat4 proj, view, model;

	// bind
	map_t* map;
	instance_t* inst;
	glm::vec3 pos_from_inst;

	float h_angle, v_angle; // camera angle
	float mouse_speed, move_speed; // camera movement
	double mouse_x, mouse_y;
public:
	camera_t();
	~camera_t() {}

	void bind_map(map_t *map);
	void set_pos_from_inst(const glm::vec3 &vec);
	void proj_ortho();
	void proj_persp_first();
	void proj_persp_third();
	void look_player();
	void look_anywhere_mouse(GLFWwindow *&window);
	void keyboard_moving(GLFWwindow *&window);
	void place_camera(const shader_id_t &shader_id);
};


#endif
