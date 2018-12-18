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
#include "light.h"
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

	// Import shaders
	shader_id_t shader_id;
	shader_id.load_shader("vshader.glsl", "fshader.glsl");

	// Import assets
	asset_kv_t assets;
	import_assets_initially(assets, shader_id);

	// List imports
	for (auto& i : assets) {
		auto& key = i.first; auto& val = i.second;
		std::cout
			<< "< ("
			<< key.major_type << ", " << key.minor_type
			<< "), ("
			<< val.vertex_array_id << ", " << val.vertex_array_size << ", "
			<< val.vertex_id << ", " << val.texcoord_id << ", " << val.normal_id << ", " << val.texture_id << ", "  << val.normal_map_id << ", "
			<< val.major_name.c_str() << ", " << val.minor_name.c_str() << ", " 
			<< val.x_max << ", " << val.x_min << ", " << val.y_max << ", " << val.y_min << ", " << val.z_max << ", " << val.z_min
			<< ") >\n";
	}

	map_t map;
	map.bind_assets(&assets);
	map.generate(11, 700);

	camera_t cam;
	cam.bind_map(&map);

	light_t light;
	light.bind_map(&map);
	light.pos_from_target(0, GRID_SIZE, 2 * GRID_SIZE, 1);
	light.Ia(1, 1, 1, 1);
	light.Id(1, 1, 1, 1);
	light.Is(1, 1, 1, 1);

	double last_t = 0.0f; 
	do {
		double curr_t = glfwGetTime();
		DELTA_T = float(curr_t - last_t);
		last_t = curr_t;
		glfwPollEvents();
		
		map.player_keyinput_move(window);
		map.update_vehicle();
		map.dead_check();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader_id.shader);
		// draw
		cam.change_proj(window);
		cam.look_target(window);
		cam.place(shader_id);

		light.place(shader_id);

		map.draw(shader_id);

		glfwSwapBuffers(window);
	} while (
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
		&& glfwWindowShouldClose(window) == 0);
	
	finalize();

	return 0;
}