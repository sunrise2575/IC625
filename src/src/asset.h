#ifndef __IC625_ASSET_H__
#define __IC625_ASSET_H__

#include <string>
#include <map>
#include <GL/glew.h>

class asset_key_t {
public:
	GLuint major_type, minor_type;
};

class asset_value_t {
private:
	void release();
public:
	GLuint vertex_array_id, vertex_array_size; // VAO IDs/Size
	GLuint vertex_id, texcoord_id, normal_id; // VBO IDs
	GLuint texture_id; // Texture Buffer IDs
	std::string major_name, minor_name;
	float x_max, x_min, y_max, y_min, z_max, z_min; // for collision box

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
		const std::string tex_path);
};

using asset_kv_t = std::map<asset_key_t, asset_value_t>;
bool operator<(asset_key_t const& left, asset_key_t const& right);

void import_assets_initially(asset_kv_t& assets);

#endif
