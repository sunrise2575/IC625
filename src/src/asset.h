#ifndef __IC625_ASSET_H__
#define __IC625_ASSET_H__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include "shader.h"

class asset_key_t {
public:
	GLuint major_type, minor_type;
};

class asset_value_t {
private:
	void release();
public:
	GLuint vertex_array_id, vertex_array_size; // VAO IDs/Size
	GLuint vertex_id, texcoord_id, normal_id, tangent_id, bitangent_id;
	GLuint texture_id, normal_map_id;
	std::string major_name, minor_name;
	float x_max, x_min, y_max, y_min, z_max, z_min; // for collision box
	glm::vec4 Ka, Kd, Ks;
	float Illum;

	asset_value_t();
	asset_value_t(const asset_value_t&) = delete;
	asset_value_t(asset_value_t&& other);
	~asset_value_t();

	asset_value_t& operator=(const asset_value_t&) = delete;
	asset_value_t& operator=(asset_value_t&& other);

	void import(
		const std::string major_name,
		const std::string minor_name,
		const std::string obj_path,
		const std::string mtl_path,
		const std::string tex_path,
		const std::string normal_map_path,
		const shader_id_t &shader);
};

using asset_kv_t = std::map<asset_key_t, asset_value_t>;
bool operator<(asset_key_t const& left, asset_key_t const& right);

void import_assets_initially(asset_kv_t& assets, const shader_id_t &shader);

#endif
