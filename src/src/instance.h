#ifndef __IC625_INSTANCE_H__
#define __IC625_INSTANCE_H__

#include "asset.h"
#include "shader.h"
#include <glm/glm.hpp>

class instance_t {
private:
	asset_key_t type;
	glm::vec3 pos;
	glm::mat4 trans, scale, rot;
public:
	instance_t();
	instance_t(const instance_t& other); 
	instance_t(instance_t&& other) = delete;
	~instance_t();

	instance_t& operator=(const instance_t&) = delete;
	instance_t& operator=(instance_t&& other) = delete;

	void assign_asset(const asset_key_t& type);
	void place(const glm::vec3& pos);
	void rotate_180();
	void change_scale(const glm::vec3& scale);
	void move(const glm::vec3& dv);
	glm::vec3 get_pos();
	void draw(const asset_kv_t& assets, const shader_id_t& shader_id);
};

#endif
