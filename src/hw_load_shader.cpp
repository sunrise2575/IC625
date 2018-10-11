#include "common.h"

static void load(const std::string file_path, std::string& code)
{
	std::cout << "Load " << file_path << "...";
	std::ifstream f(file_path, std::ios::in);
	if (f.is_open()) {
		std::stringstream ss;
		ss << f.rdbuf();
		code = ss.str();
		f.close();
		std::cout << "Success" << std::endl;
	} else {
		std::cout << "Failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

static void compile(const GLuint shader_id, const std::string file_path, const std::string& code) {
	int result = false;
	int result_length;

	// compile vertex shader 
	std::cout << "Compile " << file_path << "...";
	char const *p = code.c_str();
	glShaderSource(shader_id, 1, &p, nullptr);
	glCompileShader(shader_id);

	// check vertex shader 
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &result_length);

	if (result_length > 0) {
		std::cout << "Failed" << std::endl;
		std::vector<char> error_log(result_length+1);
		glGetShaderInfoLog(shader_id, result_length, nullptr, error_log.data());
		printf("%s\n", error_log.data());
		exit(EXIT_FAILURE);
	}
	std::cout << "Success" << std::endl;
}

static void link(GLuint pid, const std::vector<GLuint> shaders) {
	int result = false;
	int result_length;

	std::cout << "Link...";
	for (auto& s : shaders) { glAttachShader(pid, s); }
	glLinkProgram(pid);

	// check program 
	glGetProgramiv(pid, GL_LINK_STATUS, &result);
	glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &result_length);
	if (result_length > 0){
		std::cout << "Failed" << std::endl;
		std::vector<char> error_log(result_length+1);
		glGetProgramInfoLog(pid, result_length, nullptr, error_log.data());
		printf("%s\n", error_log.data());
		exit(EXIT_FAILURE);
	}

	for (auto& s : shaders) { glDetachShader(pid, s); }
	std::cout << "Success" << std::endl;
}

GLuint hw::load_shader(const char* vertex_file, const char* fragment_file) {
	std::vector<GLuint> shader(2);
	std::vector<std::string> code(2);

	shader[0] = glCreateShader(GL_VERTEX_SHADER);
	shader[1] = glCreateShader(GL_FRAGMENT_SHADER);

	load(vertex_file, code[0]);
	load(fragment_file, code[1]);
	compile(shader[0], vertex_file, code[0]);
	compile(shader[1], fragment_file, code[1]);

	GLuint pid = glCreateProgram();
	link(pid, shader);
	for (auto& s : shader) { glDeleteShader(s); }

	return pid;
}
