#include "light.h"
#include <glm/gtc/type_ptr.hpp>

void light_t::bind_map(const map_t *map) {
	this->_map = const_cast<map_t *>(map);
	this->_target = &this->_map->player;
}

void light_t::place(const shader_id_t &shader_id) {
	this->pos(glm::vec4(this->_target->v_trans().x, this->_target->v_trans().y, 0, 0) + this->pos_from_target());
	glUniform4fv(shader_id.pos.light, 1, glm::value_ptr(this->pos()));
	glUniform4fv(shader_id.phong.ia, 1, glm::value_ptr(this->Ia()));
	glUniform4fv(shader_id.phong.id, 1, glm::value_ptr(this->Id()));
	glUniform4fv(shader_id.phong.is, 1, glm::value_ptr(this->Is()));
}

void light_t::Ia(const glm::vec4 &val) { 
	this->_Ia = val;
}
void light_t::Id(const glm::vec4 &val) { 
	this->_Id = val;
}
void light_t::Is(const glm::vec4 &val) { 
	this->_Is = val; 
}
void light_t::pos(const glm::vec4 &val) { 
	this->_pos = val;
}
void light_t::pos_from_target(const glm::vec4 &val) {
	this->_pos_from_target = val;
}

void light_t::Ia(const float x, const float y, const float z, const float w) {
	this->_Ia = glm::vec4(x, y, z, w);
}
void light_t::Id(const float x, const float y, const float z, const float w) {
	this->_Id = glm::vec4(x, y, z, w);
}
void light_t::Is(const float x, const float y, const float z, const float w) {
	this->_Is = glm::vec4(x, y, z, w);
}
void light_t::pos(const float x, const float y, const float z, const float w) {
	this->_pos = glm::vec4(x, y, z, w);
}
void light_t::pos_from_target(const float x, const float y, const float z, const float w) {
	this->_pos_from_target = glm::vec4(x, y, z, w);
}

const glm::vec4 &light_t::Ia() { return this->_Ia; }
const glm::vec4 &light_t::Id() { return this->_Id; }
const glm::vec4 &light_t::Is() { return this->_Is; }
const glm::vec4 &light_t::pos() { return this->_pos; }
const glm::vec4 &light_t::pos_from_target() { return this->_pos_from_target; }
