#include "common.h"

void hw::glew::quit(const char* msg) {
	fprintf(stderr, "GLEW failure: %s\n", msg);
	glfwTerminate();
	exit(EXIT_FAILURE);
}


void hw::glew::init() {
	glewExperimental = true;
	if (glewInit() != GLEW_OK) hw::glew::quit("initialize");
	if (!GLEW_VERSION_3_3) hw::glew::quit("OpenGL 3.3 API unavailable");
}
