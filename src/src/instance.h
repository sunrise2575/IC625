#ifndef __IC625_INSTANCE_H__
#define __IC625_INSTANCE_H__

#include "asset.h"
#include "shader.h"
#include <glm/glm.hpp>

class instance_t {
private:
	asset_key_t type;
	glm::vec3 pos_vec, scale_vec;
	glm::mat4 trans_mat, scale_mat, rot_mat;
public:
	instance_t();
	instance_t(const instance_t& other); 
	instance_t(instance_t&& other) = delete;
	~instance_t() = default;

	instance_t& operator=(const instance_t&) = delete;
	instance_t& operator=(instance_t&& other) = delete;

	void assign_asset(const asset_key_t& type);
	void place(const glm::vec3& pos);
	void scale(const glm::vec3& scale);
	void move(const glm::vec3& velocity);
	void rotate_180();
	glm::vec3 get_pos();
	glm::vec3 get_scale();
	asset_key_t get_asset_key();
	void draw(const asset_kv_t& assets, const shader_id_t& shader_id);
};

#endif
