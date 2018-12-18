#ifndef __IC625_CAMERA_H__
#define __IC625_CAMERA_H__

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include "scene_graph.h"
//#include "instance.h"
#include "shader.h"

class map_t;

class camera_t {
private:
	// camera position, direction, right and up
	glm::vec3 pos; 
	glm::mat4 proj, view;

	int mode;

	// bind
	map_t* map;
	//instance_t* target;
	scene_graph_t* target;
	glm::vec3 pos_from_target;

	float h_angle, v_angle; // camera angle
	float mouse_speed, move_speed; // camera movement
	double mouse_x, mouse_y;
	void proj_ortho();
	void proj_persp_first();
	void proj_persp_third();
public:
	camera_t();
	~camera_t() = default;
	camera_t(const camera_t &) = delete;
	camera_t(camera_t &&) = delete;
	camera_t &operator=(const camera_t &) = delete;
	camera_t &operator=(camera_t &&) = delete;

	void bind_map(const map_t *map);
	void look_target(GLFWwindow *&window);
	void place(const shader_id_t &shader_id);
	void change_proj(GLFWwindow *&window);
};


#endif
