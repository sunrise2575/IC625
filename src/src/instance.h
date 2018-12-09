#ifndef __IC625_INSTANCE_H__
#define __IC625_INSTANCE_H__

#include "asset.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <vector>

class instance_t {
protected:
	asset_key_t _asset; // key for polygon data
	glm::vec3 _v_trans, _v_scale;
	glm::mat4 _m_trans, _m_scale, _m_rot;

public:
	instance_t();
	instance_t(const instance_t &); 
	instance_t(instance_t &&) = delete;
	~instance_t() = default;
	instance_t& operator=(const instance_t &) = delete;
	instance_t& operator=(instance_t &&) = delete;
	
	// asset
	void asset(const asset_key_t &);
	asset_key_t asset();

	// change vectors and matrices
	void v_trans(const glm::vec3 &);
	void v_trans(const float x, const float y, const float z);
	void v_trans_rel(const glm::vec3 &);
	void v_trans_rel(const float x, const float y, const float z);
	void v_trans_vel(const glm::vec3 &velocity);
	void v_trans_vel(const float x, const float y, const float z);
	void v_scale(const glm::vec3 &);
	void v_scale(const float x, const float y, const float z);
	glm::vec3 v_trans();
	glm::vec3 v_scale();

	void m_trans(const glm::mat4 &);
	void m_rot(const glm::mat4 &);
	void m_scale(const glm::mat4 &);
	glm::mat4 m_trans();
	glm::mat4 m_rot();
	glm::mat4 m_scale();

	void rot_x_axis(const float degree);
	void rot_y_axis(const float degree);
	void rot_z_axis(const float degree);

	void draw(const asset_kv_t& assets, const shader_id_t& shader_id);
};

#endif
