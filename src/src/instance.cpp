#include "instance.h"
#include "global_var.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

instance_t::instance_t() {
	this->v_trans(0, 0, 0);
	this->v_scale(1, 1, 1);
	this->m_scale(glm::scale(glm::mat4(1.0), this->v_scale()));
	this->m_trans(glm::translate(glm::mat4(1.0), this->v_trans()));
	this->m_rot(glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0))
		* glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0)));
}

instance_t::instance_t(const instance_t& other) {
	this->_asset = other._asset;
	this->_v_trans = other._v_trans;
	this->_m_trans = other._m_trans;
	this->_v_scale = other._v_scale;
	this->_m_scale = other._m_scale;
	this->_m_rot = other._m_rot;
}

void instance_t::asset(const asset_key_t &key) {
	this->_asset = key;
}

asset_key_t instance_t::asset() {
	return this->_asset;
}

void instance_t::v_trans(const glm::vec3 &vec) {
	this->_v_trans = vec;
	this->_m_trans = glm::translate(glm::mat4(1.0), this->_v_trans);
}

void instance_t::v_trans(const float x, const float y, const float z) {
	this->_v_trans = glm::vec3(x, y, z);
	this->_m_trans = glm::translate(glm::mat4(1.0), this->_v_trans);
}

void instance_t::v_trans_rel(const glm::vec3 &vec) {
	this->_v_trans += vec;
	this->_m_trans = glm::translate(glm::mat4(1.0), this->_v_trans);
}

void instance_t::v_trans_rel(const float x, const float y, const float z) {
	this->_v_trans += glm::vec3(x, y, z);
	this->_m_trans = glm::translate(glm::mat4(1.0), this->_v_trans);
}

void instance_t::v_trans_vel(const glm::vec3 &velocity) {
	glm::vec3 delta = velocity * DELTA_T;
	this->_v_trans += delta;
	this->_m_trans = glm::translate(glm::mat4(1.0), this->_v_trans);
}

void instance_t::v_trans_vel(const float x, const float y, const float z) {
	glm::vec3 delta = glm::vec3(x, y, z) * DELTA_T;
	this->_v_trans += delta;
	this->_m_trans = glm::translate(glm::mat4(1.0), this->_v_trans);
}

void instance_t::v_scale(const glm::vec3 &vec) {
	this->_v_scale = vec;
	this->_m_scale = glm::scale(glm::mat4(1.0), this->_v_scale);
}

void instance_t::v_scale(const float x, const float y, const float z) {
	this->_v_scale = glm::vec3(x, y, z);
	this->_m_scale = glm::scale(glm::mat4(1.0), this->_v_scale);
}

glm::vec3 instance_t::v_trans() {
	return this->_v_trans;
}

glm::vec3 instance_t::v_scale() {
	return this->_v_scale;
}

void instance_t::rot_x_axis(const float degree) {
	this->_m_rot = glm::rotate(glm::mat4(1.0), glm::radians(degree), glm::vec3(1.0, 0.0, 0.0)) * this->_m_rot;
}

void instance_t::rot_y_axis(const float degree) {
	this->_m_rot = glm::rotate(glm::mat4(1.0), glm::radians(degree), glm::vec3(0.0, 1.0, 0.0)) * this->_m_rot;
}

void instance_t::rot_z_axis(const float degree) {
	this->_m_rot = glm::rotate(glm::mat4(1.0), glm::radians(degree), glm::vec3(0.0, 0.0, 1.0)) * this->_m_rot;
}

void instance_t::m_trans(const glm::mat4 &mat) {
	this->_m_trans = mat;
}

void instance_t::m_rot(const glm::mat4 &mat) {
	this->_m_rot = mat;
}

void instance_t::m_scale(const glm::mat4 &mat) {
	this->_m_scale = mat;
}

glm::mat4 instance_t::m_trans() {
	return this->_m_trans;
}

glm::mat4 instance_t::m_rot() {
	return this->_m_rot;
}

glm::mat4 instance_t::m_scale() {
	return this->_m_scale;
}

void instance_t::draw(const asset_kv_t& assets, const shader_id_t& shader_id) {
	const auto& data = assets.find(this->_asset)->second;
	
	// Apply Transform
	glUniformMatrix4fv(shader_id.mat.scale, 1, GL_FALSE, glm::value_ptr(this->m_scale()));
	glUniformMatrix4fv(shader_id.mat.rot, 1, GL_FALSE, glm::value_ptr(this->m_rot()));
	glUniformMatrix4fv(shader_id.mat.trans, 1, GL_FALSE, glm::value_ptr(this->m_trans()));

	glUniform4fv(shader_id.phong.ka, 1, glm::value_ptr(data.Ka));
	glUniform4fv(shader_id.phong.kd, 1, glm::value_ptr(data.Kd));
	glUniform4fv(shader_id.phong.ks, 1, glm::value_ptr(data.Ks));
	glUniform1f(shader_id.phong.shininess, data.Illum);

	// Apply Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data.texture_id);
	glUniform1i(shader_id.sampler.texture, 0);

	// Apply normal map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, data.normal_map_id);
	glUniform1i(shader_id.sampler.normap, 1);

	// Apply Vertex Info
	glBindVertexArray(data.vertex_array_id);
	
	// Draw
	glDrawArrays(GL_TRIANGLES, 0, data.vertex_array_size);
	
	// Detach Vertex Info
	glBindVertexArray(0);
}
