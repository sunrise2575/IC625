#include <vector>
#include <glm/glm.hpp>
#include <filesystem>

#include "asset.h"
#include "object_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void asset_value_t::release() {
	if (this->vertex_id) glDeleteBuffers(1, &this->vertex_id);
	if (this->texcoord_id) glDeleteBuffers(1, &this->texcoord_id);
	if (this->normal_id) glDeleteBuffers(1, &this->normal_id);
	if (this->texture_id) glDeleteTextures(1, &this->texture_id);

	if (this->vertex_array_id) glDeleteVertexArrays(1, &this->vertex_array_id);
}

asset_value_t::asset_value_t() {
	glGenVertexArrays(1, &this->vertex_array_id);

	glGenBuffers(1, &this->vertex_id);
	glGenBuffers(1, &this->texcoord_id);
	glGenBuffers(1, &this->normal_id);
	glGenTextures(1, &this->texture_id);
}

asset_value_t::asset_value_t(asset_value_t&& other) {
	this->vertex_array_id = other.vertex_array_id;
	this->vertex_id = other.vertex_id;
	this->texcoord_id = other.texcoord_id;
	this->normal_id = other.normal_id;
	this->texture_id = other.texture_id;
	other.vertex_array_id = 0;
	other.vertex_id = 0;
	other.texcoord_id = 0;
	other.normal_id = 0;
	other.texture_id = 0;

	this->vertex_array_size = other.vertex_array_size;
	this->major_name = other.major_name;
	this->minor_name = other.minor_name;
}

asset_value_t& asset_value_t::operator=(asset_value_t&& other) {
	if (this != &other) {
		this->release();

		this->vertex_array_id = other.vertex_array_id;
		this->vertex_id = other.vertex_id;
		this->texcoord_id = other.texcoord_id;
		this->normal_id = other.normal_id;
		this->texture_id = other.texture_id;
		other.vertex_array_id = 0;
		other.vertex_id = 0;
		other.texcoord_id = 0;
		other.normal_id = 0;
		other.texture_id = 0;

		this->vertex_array_size = other.vertex_array_size;
		this->major_name = other.major_name;
		this->minor_name = other.minor_name;
	}

	return *this;
}

asset_value_t::~asset_value_t() {
	this->release();
}

void asset_value_t::import(
	const std::string major_name,
	const std::string minor_name,
	const std::string obj_path,
	const std::string tex_path)
{
	std::vector<glm::vec3> vertex;
	std::vector<glm::vec2> texcoord;
	std::vector<glm::vec3> normal;
	if (load_object(obj_path.c_str(), vertex, texcoord, normal)) {
		fprintf(stdout, "Imported: %s\n", obj_path.c_str());
	}
	else {
		fprintf(stdout, "Import failure: %s\n", obj_path.c_str());
		return;
	}

	this->vertex_array_size = vertex.size();

	glBindVertexArray(this->vertex_array_id);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_id);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), vertex.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->texcoord_id);
	glBufferData(GL_ARRAY_BUFFER, texcoord.size() * sizeof(glm::vec2), texcoord.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->normal_id);
	glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(glm::vec3), normal.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	//glBindVertexArray(0);
	// Load Texture
	glBindTexture(GL_TEXTURE_2D, this->texture_id);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int width, height, channels;
	unsigned char *data = stbi_load(tex_path.c_str(), &width, &height, &channels, STBI_rgb);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		fprintf(stdout, "Imported: %s\n", tex_path.c_str());
	}
	else {
		fprintf(stdout, "Import failure: %s (%s)\n", tex_path.c_str(), stbi_failure_reason());
		std::exit(EXIT_FAILURE);
	}

	stbi_image_free(data);

	this->major_name = major_name;
	this->minor_name = minor_name;

}

bool operator<(asset_key_t const& left, asset_key_t const& right) {
	if (left.major_type < right.major_type) return true;
	if (left.major_type > right.major_type) return false;
	return left.minor_type < right.minor_type;
}

void import_assets_initially(asset_kv_t& assets) {
	// the key is starting from 1
	// Count files
	namespace fs = std::filesystem;
	std::string base("assets");
	// Firstly, searching files

	GLuint MAJOR_NUMBER = 1;
	for (auto& subdir : fs::directory_iterator(base)) {
		// Extract only subdirectory that is not empty
		if (fs::is_directory(subdir) && !fs::is_empty(subdir)) {
			bool is_file_exist_in_subdir = false;
			GLuint MINOR_NUMBER = 1;
			for (auto& objfile : fs::directory_iterator(subdir)) {
				// the file/folder is not folder nor empty
				if (!fs::is_directory(objfile) && !fs::is_empty(objfile)) {
					if (std::string(".obj") == objfile.path().extension().string()) {
						fs::path texture_path = objfile.path(); //make lvalue
						texture_path.replace_extension(".png");

						if (fs::exists(texture_path)) {
							if (!is_file_exist_in_subdir) { is_file_exist_in_subdir = true; }
							// Finally this file is worth to register!
							std::string major_type = subdir.path().stem().string();
							std::string minor_type = objfile.path().stem().string();
							std::string object = objfile.path().string();
							std::string texture = texture_path.string();

							// find last number of 
							asset_key_t key{MAJOR_NUMBER, MINOR_NUMBER};
							asset_value_t value;
							value.import(major_type, minor_type, object, texture);
							assets.insert(std::make_pair(key, std::move(value)));
							MINOR_NUMBER++;
						}
					}

				}
			}
			if (is_file_exist_in_subdir) { MAJOR_NUMBER++; }
		}
	}
}