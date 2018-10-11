#include "common.h"

void hw::glfw::version(const float version) {
	const int major = (int)version;
	const int minor = (int)(10.0 * (version - (float)major));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);

	if (major >= 3) {
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}
	
	if (major >= 3 && minor >= 3) {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	} else {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	}
}

void hw::glfw::callback::error(int error_code, const char* error_description) {
	fprintf(stderr, "Error: Code %d, %s\n", error_code, error_description);
}

void hw::glfw::callback::key(
		GLFWwindow* window,
		int key,
		int /*scancode*/,
		int action,
		int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	hw::player.key_input(key, action);
}

void hw::glfw::callback::resize(GLFWwindow* /*window*/, int width, int height) {
	glViewport(0, 0, width, height);
	hw::CAMERA_WIDTH = width;
	hw::CAMERA_HEIGHT = height;
	hw::GRID_WIDTH = CAMERA_HEIGHT / GRIDS;
}

void hw::glfw::quit(const char* msg) {
	fprintf(stderr, "GLFW failure: %s\n", msg);
	glfwTerminate();
	exit(EXIT_FAILURE);
}

void hw::glfw::init() {
	glfwSetErrorCallback(hw::glfw::callback::error);
	if (!glfwInit()) {
		fprintf(stderr, "GLFW failure: initialize\n");
		exit(EXIT_FAILURE);
	}

	hw::glfw::version(3.3);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // fix screen size
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	hw::window = glfwCreateWindow(hw::CAMERA_WIDTH, hw::CAMERA_HEIGHT, "Homework 01", nullptr, nullptr);
	if (!hw::window) hw::glfw::quit("Can't open window");

	glfwMakeContextCurrent(hw::window);
	glfwSetKeyCallback(hw::window, hw::glfw::callback::key);
	glfwSetWindowSizeCallback(hw::window, hw::glfw::callback::resize);
	glfwSwapInterval(1); // VSync
}
