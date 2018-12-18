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
	if (this->tangent_id) glDeleteBuffers(1, &this->tangent_id);
	if (this->bitangent_id) glDeleteBuffers(1, &this->bitangent_id);
	
	if (this->vertex_array_id) glDeleteVertexArrays(1, &this->vertex_array_id);

	if (this->texture_id) glDeleteTextures(1, &this->texture_id);
	if (this->normal_map_id) glDeleteTextures(1, &this->normal_map_id);
}

asset_value_t::asset_value_t() {
	//memset(this, 0, sizeof(*this));

	glGenVertexArrays(1, &this->vertex_array_id);

	glGenBuffers(1, &this->vertex_id);
	glGenBuffers(1, &this->texcoord_id);
	glGenBuffers(1, &this->normal_id);
	glGenBuffers(1, &this->tangent_id);
	glGenBuffers(1, &this->bitangent_id);

	glGenTextures(1, &this->texture_id);
	glGenTextures(1, &this->normal_map_id);
}

asset_value_t::asset_value_t(asset_value_t&& other) {
	this->vertex_array_id = other.vertex_array_id;
	this->vertex_id = other.vertex_id;
	this->texcoord_id = other.texcoord_id;
	this->tangent_id = other.tangent_id;
	this->bitangent_id = other.bitangent_id;
	this->normal_map_id = other.normal_map_id;
	this->normal_id = other.normal_id;
	this->texture_id = other.texture_id;

	other.vertex_array_id = 0;
	other.vertex_id = 0;
	other.texcoord_id = 0;
	other.normal_id = 0;
	other.tangent_id = 0;
	other.bitangent_id = 0;
	other.texture_id = 0;
	other.normal_map_id = 0;

	this->vertex_array_size = other.vertex_array_size;
	this->major_name = other.major_name;
	this->minor_name = other.minor_name;
	this->x_max = other.x_max;
	this->x_min = other.x_min;
	this->y_max = other.y_max;
	this->y_min = other.y_min;
	this->z_max = other.z_max;
	this->z_min = other.z_min;
	this->Ka = other.Ka;
	this->Kd = other.Kd;
	this->Ks = other.Ks;
	this->Illum = other.Illum;
}

asset_value_t& asset_value_t::operator=(asset_value_t&& other) {
	if (this != &other) {
		this->release();

		this->vertex_array_id = other.vertex_array_id;
		this->vertex_id = other.vertex_id;
		this->texcoord_id = other.texcoord_id;
		this->tangent_id = other.tangent_id;
		this->bitangent_id = other.bitangent_id;
		this->normal_map_id = other.normal_map_id;
		this->normal_id = other.normal_id;
		this->texture_id = other.texture_id;

		other.vertex_array_id = 0;
		other.vertex_id = 0;
		other.texcoord_id = 0;
		other.normal_id = 0;
		other.tangent_id = 0;
		other.bitangent_id = 0;
		other.texture_id = 0;
		other.normal_map_id = 0;

		this->vertex_array_size = other.vertex_array_size;
		this->major_name = other.major_name;
		this->minor_name = other.minor_name;
		this->x_max = other.x_max;
		this->x_min = other.x_min;
		this->y_max = other.y_max;
		this->y_min = other.y_min;
		this->z_max = other.z_max;
		this->z_min = other.z_min;
		this->Ka = other.Ka;
		this->Kd = other.Kd;
		this->Ks = other.Ks;
		this->Illum = other.Illum;
	}

	return *this;
}

asset_value_t::~asset_value_t() {
	this->release();
}
	// Load Texture

void asset_value_t::import(
	const std::string major_name,
	const std::string minor_name,
	const std::string obj_path,
	const std::string mtl_path,
	const std::string tex_path,
	const std::string normal_map_path,
	const shader_id_t &shader)
{
	std::vector<glm::vec3> vertex;
	std::vector<glm::vec2> texcoord;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> bitangent;

	// load vertex/texcoord/normal from *.obj file
	if (load_object(obj_path.c_str(), vertex, texcoord, normal)) {
		fprintf(stdout, "Imported: %s\n", obj_path.c_str());
	}
	else {
		fprintf(stdout, "Import failure: %s\n", obj_path.c_str());
		return;
	}

	if (load_KaKdKsIllum(mtl_path.c_str(), this->Ka, this->Kd, this->Ks, this->Illum)) {
		fprintf(stdout, "Imported: %s\n", mtl_path.c_str());
	}
	else {
		fprintf(stdout, "Import failure: %s\n", obj_path.c_str());
		return;
	}

	// calculate tangent/bitangent by vertex/texcoord/normal
	/*
	[&]{
		tangent.resize(vertex.size());
		bitangent.resize(vertex.size());

		const auto vertex_sz = vertex.size();
		for (size_t i = 0; i < vertex_sz; i += 3) {
			const glm::vec3 &v0 = vertex[i], &v1 = vertex[i + 1], &v2 = vertex[i + 2];
			const glm::vec2 &uv0 = texcoord[i], &uv1 = texcoord[i + 1], &uv2 = texcoord[i + 2];

			const glm::vec3 &&e1 = v1 - v0, &&e2 = v2 - v0;
			const glm::vec2 &&duv1 = uv1 - uv0, &&duv2 = uv2 - uv0;

			float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);
			tangent[i] = tangent[i + 1] = tangent[i + 2]
				= f * (duv2.y * e1 - duv1.y * e2);
			bitangent[i] = bitangent[i + 1] = bitangent[i + 2]
				= f * (-duv2.x * e1 + duv1.x * e2);
		}
	}();
	*/

	this->vertex_array_size = (GLuint)vertex.size();

	glBindVertexArray(this->vertex_array_id);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_id);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), vertex.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(shader.coord.vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader.coord.vertex);

	glBindBuffer(GL_ARRAY_BUFFER, this->texcoord_id);
	glBufferData(GL_ARRAY_BUFFER, texcoord.size() * sizeof(glm::vec2), texcoord.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(shader.coord.texture, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader.coord.texture);

	glBindBuffer(GL_ARRAY_BUFFER, this->normal_id);
	glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(glm::vec3), normal.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(shader.coord.normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader.coord.normal);

	glBindBuffer(GL_ARRAY_BUFFER, this->tangent_id);
	glBufferData(GL_ARRAY_BUFFER, tangent.size() * sizeof(glm::vec3), tangent.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(shader.coord.tangent, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader.coord.tangent);

	glBindBuffer(GL_ARRAY_BUFFER, this->bitangent_id);
	glBufferData(GL_ARRAY_BUFFER, bitangent.size() * sizeof(glm::vec3), bitangent.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(shader.coord.bitangent, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader.coord.bitangent);

	glBindVertexArray(0);

	// major/minor object type
	this->major_name = major_name;
	this->minor_name = minor_name;

	// min/max for collision box
	auto x_result = std::minmax_element(vertex.begin(), vertex.end(), [&](glm::vec3 &a, glm::vec3 &b) { return a.x > b.x; });
	this->x_max = x_result.first->x;
	this->x_min = x_result.second->x;

	auto y_result = std::minmax_element(vertex.begin(), vertex.end(), [&](glm::vec3 &a, glm::vec3 &b) { return a.y > b.y; });
	this->y_max = y_result.first->y;
	this->y_min = y_result.second->y;

	auto z_result = std::minmax_element(vertex.begin(), vertex.end(), [&](glm::vec3 &a, glm::vec3 &b) { return a.z > b.z; });
	this->z_max = z_result.first->z;
	this->z_min = z_result.second->z;

	auto _texture_loader = [&](const decltype(GL_TEXTURE0) gl_texture_num, const std::string &path, const GLuint &buffer_id)
	{
		glActiveTexture(gl_texture_num);
		glBindTexture(GL_TEXTURE_2D, buffer_id);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		int width, height, channels;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			fprintf(stdout, "Imported: %s\n", path.c_str());
		}
		else {
			fprintf(stdout, "Import failure: %s (%s)\n", path.c_str(), stbi_failure_reason());
		}
		stbi_image_free(data);
	};

	_texture_loader(GL_TEXTURE0, tex_path, this->texture_id);
	_texture_loader(GL_TEXTURE1, normal_map_path, this->normal_map_id);
}

bool operator<(asset_key_t const& left, asset_key_t const& right) {
	if (left.major_type < right.major_type) return true;
	if (left.major_type > right.major_type) return false;
	return left.minor_type < right.minor_type;
}

void import_assets_initially(asset_kv_t& assets, const shader_id_t &shader) {
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
						fs::path material_path = objfile.path();
						fs::path texture_path = objfile.path(); //make lvalue
						fs::path normal_map_path = objfile.path();
						material_path.replace_extension(".mtl");
						normal_map_path.replace_extension(".jpg");
						texture_path.replace_extension(".png");

						if (fs::exists(texture_path)) {
							if (!is_file_exist_in_subdir) { is_file_exist_in_subdir = true; }
							// Finally this file is worth to register!
							std::string major_type = subdir.path().stem().string();
							std::string minor_type = objfile.path().stem().string();
							std::string object = objfile.path().string();
							std::string material = material_path.string();
							std::string texture = texture_path.string();
							std::string normal_map("");
							if (fs::exists(normal_map_path)) {
								normal_map = normal_map_path.string();
							}

							// find last number of 
							asset_key_t key{MAJOR_NUMBER, MINOR_NUMBER};
							asset_value_t value;
							value.import(major_type, minor_type, object, material, texture, normal_map, shader);
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