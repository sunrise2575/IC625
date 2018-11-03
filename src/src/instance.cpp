#include "instance.h"
#include "global_var.h"
#include <glm/gtc/matrix_transform.hpp>

instance_t::instance_t() {
	this->pos = glm::vec3(0.0);

	this->scale = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
	this->rot = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	this->rot *= glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	this->trans = glm::translate(glm::mat4(1.0), this->pos);
}

instance_t::instance_t(const instance_t& other) {
	this->pos = other.pos;
	this->type = other.type;
	this->trans = other.trans;
	this->scale = other.scale;
	this->rot = other.rot;
}

instance_t::~instance_t() { }

void instance_t::place(const glm::vec3& pos) {
	this->pos = pos;
	this->trans = glm::translate(glm::mat4(1.0), this->pos);
}

glm::vec3 instance_t::get_pos() {
	return this->pos;
}

void instance_t::rotate_180() {
	this->rot = glm::rotate(this->rot, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
}

void instance_t::change_scale(const glm::vec3 &scale) {
	this->scale = glm::scale(glm::mat4(1.0), scale);
}

void instance_t::move(const glm::vec3 &dv) {
	glm::vec3 delta = dv * DELTA_T;
	this->pos += delta;
	this->trans = glm::translate(glm::mat4(1.0),
		delta) * this->trans;
}

void instance_t::assign_asset(const asset_key_t& type) {
	this->type = std::move(type);
}

void instance_t::draw(const asset_kv_t& assets, const shader_id_t& shader_id) {
	const auto& data = assets.find(this->type)->second;
	
	// Apply Transform
	glUniformMatrix4fv(shader_id.scale_mat, 1, GL_FALSE, &this->scale[0][0]);
	glUniformMatrix4fv(shader_id.rot_mat, 1, GL_FALSE, &this->rot[0][0]);
	glUniformMatrix4fv(shader_id.trans_mat, 1, GL_FALSE, &this->trans[0][0]);

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
