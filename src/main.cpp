#include "common.h"

namespace hw {
	GLFWwindow* window;
	GLuint CAMERA_HEIGHT = 600;
	GLuint CAMERA_WIDTH = CAMERA_HEIGHT * 2;
	GLuint GRID_WIDTH = CAMERA_HEIGHT / GRIDS;
	player_t player;
	std::list<hw::tile_line_t*> map;
	std::mutex map_mtx;
}

int main() {
	// Initialize
	hw::glfw::init();
	hw::glew::init();
	hw::set_shape();

	// Load shader
	GLuint shader_pid = hw::load_shader(
			"vertex_shader.glsl", "fragment_shader.glsl");

	// Generate VAO
	GLuint VAO_player, VAO_tree, VAO_car;
	glGenVertexArrays(1, &VAO_player);
	glGenVertexArrays(1, &VAO_tree);
	glGenVertexArrays(1, &VAO_car);

	// Create player object
	hw::player.set_vertex(hw::shape::square_vertex);
	hw::player.set_color(hw::shape::square_color);
	hw::player.set_VAO(VAO_player);
	hw::player.set_shader(shader_pid);
	hw::player.bind_vbo_to_vao();

	// Place player object
	hw::player.scale(0.5f, 0.5f);
	hw::player.place_on_grid(0, hw::GRIDS / 2);

	// starting place is no tree plain
	hw::map.push_back(new hw::ground_t(0));
	hw::player.set_starting_line();

	// further tile_line will be added on the main loop.
	for (int i = 1; i < 100; i++) {
		if (hw::random::binary(0.4)) {
			hw::map.push_back(new hw::ground_t(i));
			hw::map.back()->gen(shader_pid, VAO_tree);
		} else {
			hw::map.push_back(new hw::road_t(i));
		}
	}

	hw::player.set_view(
		glm::vec3(8.0f * (float)hw::GRID_WIDTH, 0, 1), // camera is on (0,0,1)
		glm::vec3(8.0f * (float)hw::GRID_WIDTH, 0, 1) + glm::vec3(0, 0, -1), // camera looks to (0,0,-1)
		glm::vec3(0, 1, 0) // head is upper (0,1,0)
	);
	hw::player.set_mat_model(glm::mat4(1.0f));

	while (!glfwWindowShouldClose(hw::window)) {
		glUseProgram(shader_pid);

		// color buffer
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Model View Projection
		hw::player.set_mat_proj(
			glm::ortho(
					-(float)hw::CAMERA_WIDTH / 2.0f, (float)hw::CAMERA_WIDTH / 2.0f,
					-(float)hw::CAMERA_HEIGHT/ 2.0f, (float)hw::CAMERA_HEIGHT/ 2.0f,
					0.0f, 100.0f)
			);

		glm::mat4 mvp = hw::player.get_camera();
		GLuint model_view_proj = glGetUniformLocation(shader_pid, "model_view_proj");
		glUniformMatrix4fv(model_view_proj, 1, GL_FALSE, glm::value_ptr(mvp));

		// collision check
		
		// generate new car
		for (auto& i : hw::map) { 
			if (i->type() == hw::tile_line_type::road) {
				i->gen(shader_pid, VAO_car);
			}
			else if (i->type() == hw::tile_line_type::ground) {
				if (i->get_index() != 0) {
					i->gen(shader_pid, VAO_tree);
				}
			}
		}

		// move car 
		for (auto& i : hw::map) { i->move(); }

		// DRAW
		hw::player.draw();
		for (auto& i : hw::map) { i->draw(); }

		// swap buffer & I/O event handle
		glfwSwapBuffers(hw::window);
		glfwPollEvents();
		// delete out-of-screen tile_line
		/*
		if (hw::map.front()->get_index() < hw::player.get_grid().x - 3) {
			if (hw::map.front() != nullptr) delete hw::map.front();
			hw::map.pop_front();
		}
		*/
		hw::player.check_dead();
		//for (auto& i : hw::map) { i->remove(); }
	}
	for (auto& i : hw::map) { if (i != nullptr) delete i; }
	glDeleteProgram(shader_pid);
	glDeleteVertexArrays(1, &VAO_player);
	glDeleteVertexArrays(1, &VAO_tree);
	glDeleteVertexArrays(1, &VAO_car);
	glfwTerminate();

	return 0;
}
