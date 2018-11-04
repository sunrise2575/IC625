#include "instance.h"
#include "global_var.h"
#include <glm/gtc/matrix_transform.hpp>

instance_t::instance_t() {
	this->pos_vec = glm::vec3(0.0);
	this->scale_vec = glm::vec3(1.0);

	this->scale_mat = glm::scale(glm::mat4(1.0), this->scale_vec);
	this->rot_mat = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	this->rot_mat *= glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	this->trans_mat = glm::translate(glm::mat4(1.0), this->pos_vec);
}

instance_t::instance_t(const instance_t& other) {
	this->type = other.type;

	this->pos_vec = other.pos_vec;
	this->scale_vec = other.scale_vec;

	this->trans_mat = other.trans_mat;
	this->scale_mat = other.scale_mat;
	this->rot_mat = other.rot_mat;
}

void instance_t::assign_asset(const asset_key_t& type) {
	this->type = std::move(type);
}

void instance_t::place(const glm::vec3& pos) {
	this->pos_vec = std::move(pos);
	this->trans_mat = glm::translate(glm::mat4(1.0), this->pos_vec);
}

void instance_t::scale(const glm::vec3 &scale) {
	this->scale_vec = std::move(scale);
	this->scale_mat = glm::scale(glm::mat4(1.0), this->scale_vec);
}

void instance_t::move(const glm::vec3 &velocity) {
	glm::vec3 delta = velocity * DELTA_T;
	this->pos_vec += delta;
	this->trans_mat = glm::translate(glm::mat4(1.0), delta) * this->trans_mat;
}

void instance_t::rotate_180() {
	this->rot_mat = glm::rotate(this->rot_mat, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
}

glm::vec3 instance_t::get_pos() {
	return this->pos_vec;
}

glm::vec3 instance_t::get_scale() {
	return this->scale_vec;
}

asset_key_t instance_t::get_asset_key() {
	return this->type;
}

void instance_t::draw(const asset_kv_t& assets, const shader_id_t& shader_id) {
	const auto& data = assets.find(this->type)->second;
	
	// Apply Transform
	glUniformMatrix4fv(shader_id.scale_mat, 1, GL_FALSE, &this->scale_mat[0][0]);
	glUniformMatrix4fv(shader_id.rot_mat, 1, GL_FALSE, &this->rot_mat[0][0]);
	glUniformMatrix4fv(shader_id.trans_mat, 1, GL_FALSE, &this->trans_mat[0][0]);

	// Apply Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data.texture_id);
	glUniform1i(shader_id.tex_sampler, 0);

	// Apply Vertex Info
	glBindVertexArray(data.vertex_array_id);
	
	// Draw
	glDrawArrays(GL_TRIANGLES, 0, data.vertex_array_size);
	
	// Detach Vertex Info
	glBindVertexArray(0);
}
