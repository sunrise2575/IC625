#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "asset.h"
#include "camera.h"
#include "instance.h"
#include "map.h"
#include "global_var.h"

static void initialize(GLFWwindow *&window) {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		std::exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Crossy Road", NULL, NULL))) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	glfwSwapInterval(1); // vsync

	// Background color
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

}

static void finalize() {
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}


int main() {
	GLFWwindow* window;
	initialize(window);

	// Import assets
	asset_kv_t assets;
	import_assets_initially(assets);

	// Import shaders
	shader_id_t shader_id;
	shader_id.load_shader("transform_vertex_shader.glsl", "texture_fragment_shader.glsl");

	// List imports
	for (auto& i : assets) {
		auto& key = i.first; auto& val = i.second;
		fprintf(stdout, "<(%d, %d), (%d, %d, %d, %d, %d, %d, %s, %s)>\n",
			key.major_type, key.minor_type,
			val.vertex_array_id, val.vertex_array_size, val.vertex_id, val.texcoord_id, val.normal_id, val.texture_id, val.major_name.c_str(), val.minor_name.c_str());
	}

	map_t map;
	map.bind_assets(&assets);
	map.generate(10, 700);

	camera_t cam;
	cam.bind_map(&map);
	cam.set_pos_from_inst(glm::vec3(4.0, -6.0, 7.0));
	cam.proj_ortho();

	double last_t = 0.0f; 
	do {
		float curr_t = glfwGetTime();
		DELTA_T = float(curr_t - last_t);
		last_t = curr_t;
		glfwPollEvents();
		
		map.player_keyinput_move(window);
		//cam.keyboard_moving(window);
		//cam.look_anywhere_mouse(window);
		cam.look_player();
		// camera position and view direction by key/mouse input
		cam.place_camera(shader_id);
		
		map.update_vehicle();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader_id.shader);
		// draw
		map.draw(shader_id);
		//map.print();

		glfwSwapBuffers(window);
	} while (
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
		&& glfwWindowShouldClose(window) == 0);
	
	finalize();

	return 0;
}