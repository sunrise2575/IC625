#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.h"

void shader_id_t::load_shader(const char * vertex_file_path, const char * fragment_file_path) {
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		std::exit(EXIT_FAILURE);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	this->shader = ProgramID;

	// coordinations
	this->coord.vertex = glGetAttribLocation(this->shader, "coord.vertex");
	this->coord.texture = glGetAttribLocation(this->shader, "coord.texture");
	this->coord.normal = glGetAttribLocation(this->shader, "coord.normal");
	this->coord.tangent = glGetAttribLocation(this->shader, "coord.tangent");
	this->coord.bitangent = glGetAttribLocation(this->shader, "coord.bitangent");

	// matrices
	this->mat.trans = glGetUniformLocation(this->shader, "mat.trans");
	this->mat.rot = glGetUniformLocation(this->shader, "mat.rot");
	this->mat.scale = glGetUniformLocation(this->shader, "mat.scale");
	this->mat.view = glGetUniformLocation(this->shader, "mat.view");
	this->mat.proj = glGetUniformLocation(this->shader, "mat.proj");

	// phong variables
	this->phong.ka = glGetUniformLocation(this->shader, "phong.ka");
	this->phong.kd = glGetUniformLocation(this->shader, "phong.kd");
	this->phong.ks = glGetUniformLocation(this->shader, "phong.ks");
	this->phong.ia = glGetUniformLocation(this->shader, "phong.ia");
	this->phong.id = glGetUniformLocation(this->shader, "phong.id");
	this->phong.is = glGetUniformLocation(this->shader, "phong.is");
	this->phong.shininess = glGetUniformLocation(this->shader, "phong.shininess");

	// positions
	this->pos.light = glGetUniformLocation(this->shader, "pos.light");
	this->pos.eye = glGetUniformLocation(this->shader, "pos.eye");

	// sampler
	this->sampler.texture = glGetUniformLocation(this->shader, "sampler.texture");
	this->sampler.normap = glGetUniformLocation(this->shader, "sampler.normap");
}
