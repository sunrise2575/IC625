#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "global_var.h"
#include "scene_graph.h"

scene_graph_elem_t::scene_graph_elem_t() {
	this->v_trans(0, 0, 0);
	this->v_scale(1, 1, 1);
	this->m_rot(glm::mat4(1));
	this->m_scale(glm::scale(glm::mat4(1.0), this->v_scale()));
	this->m_trans(glm::translate(glm::mat4(1.0), this->v_trans()));
	this->child(nullptr);
	this->sibling(nullptr);
}

scene_graph_elem_t::scene_graph_elem_t(const scene_graph_elem_t &other) {
	this->_asset = other._asset;
	this->_v_trans = other._v_trans;
	this->_v_scale = other._v_scale;
	this->_m_trans = other._m_trans;
	this->_m_rot = other._m_rot;
	this->_m_scale = other._m_scale;
	this->_child = other._child;
	this->_sibling = other._sibling;
}

scene_graph_elem_t::scene_graph_elem_t(scene_graph_elem_t &&other) {
	if (this != &other) {
		this->_asset = other._asset;

		this->_v_trans = other._v_trans;
		this->_v_scale = other._v_scale;
		this->_m_trans = other._m_trans;
		this->_m_rot = other._m_rot;
		this->_m_scale = other._m_scale;

		this->_child = other._child;
		other._child = nullptr;
		this->_sibling = other._sibling;
		other._sibling = nullptr;
	}
}

scene_graph_elem_t *scene_graph_elem_t::child() {
	return this->_child;
}

scene_graph_elem_t *scene_graph_elem_t::sibling() {
	return this->_sibling;
}

void scene_graph_elem_t::child(const scene_graph_elem_t *elem) {
	this->_child = const_cast<scene_graph_elem_t *>(elem);
}

void scene_graph_elem_t::sibling(const scene_graph_elem_t *elem) {
	this->_sibling = const_cast<scene_graph_elem_t *>(elem);
}

void scene_graph_elem_t::draw(
	const glm::mat4 &matrix, const asset_kv_t &assets, const shader_id_t &shader_id)
{
		const auto& data = assets.find(this->_asset)->second;
		
		// Apply Transform
		glUniformMatrix4fv(shader_id.mat.scale, 1, GL_FALSE, glm::value_ptr(this->m_scale()));
		glUniformMatrix4fv(shader_id.mat.rot, 1, GL_FALSE, glm::value_ptr(matrix));
		glUniformMatrix4fv(shader_id.mat.trans, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		
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

asset_key_t scene_graph_t::asset() {
	return this->_asset;
}

void scene_graph_t::asset(const asset_key_t &asset) {
	this->_asset = asset;
}

void scene_graph_t::v_trans(const float x, const float y, const float z) {
	this->_root->v_trans(x, y, z);
}

void scene_graph_t::v_trans_rel(const float x, const float y, const float z) {
	this->_root->v_trans_rel(x, y, z);
}

void scene_graph_t::v_scale(const float x, const float y, const float z) {
	this->_root->v_scale(x, y, z);
}

glm::vec3 scene_graph_t::v_trans() {
	return this->_root->v_trans();
}

glm::vec3 scene_graph_t::v_scale() {
	return this->_root->v_scale();
}

void scene_graph_t::root(const scene_graph_elem_t *root) {
	this->_root = const_cast<scene_graph_elem_t *>(root);
}

void scene_graph_t::_draw_recursive(scene_graph_elem_t *node, const asset_kv_t &assets, const shader_id_t &shader_id) {
	// pre-order tree traversal
	if (!node) return;
	this->stack.push(this->stack.top()); // PushMatrix
	this->stack.top() *= node->m_trans() * node->m_rot(); // MulMatrix
	node->draw(this->stack.top(), assets, shader_id); // Draw
	if (node->child())
		this->_draw_recursive(node->child(), assets, shader_id);
	this->stack.pop();
	if (node->sibling())
		this->_draw_recursive(node->sibling(), assets, shader_id);
}

void scene_graph_t::draw(const asset_kv_t &assets, const shader_id_t &shader_id) {
	this->animation();
	this->stack.push(glm::mat4(1.0));
	this->_draw_recursive(this->_root, assets, shader_id);
}

void scene_graph_t::simple_thing() {
	this->instances.resize(2);
	auto& p = this->instances[0];
	auto& c = this->instances[1];
	p.asset(this->asset());
	c.asset(this->asset());
	c.v_trans(0, 0, GRID_SIZE);
	p.child(&c);
	this->root(&this->instances.front());
}

void scene_graph_t::create_humanoid() {
	const float UNIT = GRID_SIZE / 8.0f;
	this->instances.resize(10);
	auto& torso = this->instances[0];
	auto& head = this->instances[1];
	auto& l_u_leg = this->instances[2];
	auto& l_l_leg = this->instances[3];
	auto& r_u_leg = this->instances[4];
	auto& r_l_leg = this->instances[5];
	auto& l_u_arm = this->instances[6];
	auto& l_l_arm = this->instances[7];
	auto& r_u_arm = this->instances[8];
	auto& r_l_arm = this->instances[9];

	for (auto &i : this->instances) {
		i.asset(this->asset());
	}

	// set scale
	torso.v_scale(4*UNIT, 8*UNIT, 2*UNIT);
	torso.m_rot(glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(1, 0, 0)));
	head.v_scale(2*UNIT, 2*UNIT, 2*UNIT);

	l_u_leg.v_scale(2*UNIT, 4*UNIT, 2*UNIT);
	r_u_leg.v_scale(2*UNIT, 4*UNIT, 2*UNIT);
	l_l_leg.v_scale(2*UNIT, 4*UNIT, 2*UNIT);
	r_l_leg.v_scale(2*UNIT, 4*UNIT, 2*UNIT);

	l_u_arm.v_scale(2*UNIT, 4*UNIT, 2*UNIT);
	r_u_arm.v_scale(2*UNIT, 4*UNIT, 2*UNIT);
	l_l_arm.v_scale(2*UNIT, 4*UNIT, 2*UNIT);
	r_l_arm.v_scale(2*UNIT, 4*UNIT, 2*UNIT);

	torso.v_trans(0, 0, 1.5f*GRID_SIZE);
	head.v_trans(0, 5*UNIT, 0);
	l_u_leg.v_trans(-1*UNIT, -6*UNIT, 0);
	r_u_leg.v_trans(1*UNIT, -6*UNIT, 0);
	l_l_leg.v_trans(0, -4*UNIT, 0);
	r_l_leg.v_trans(0, -4*UNIT, 0);
	l_u_arm.v_trans(-3*UNIT, 2*UNIT, 0);
	r_u_arm.v_trans(3*UNIT, 2*UNIT, 0);
	l_l_arm.v_trans(0, -4*UNIT, 0);
	r_l_arm.v_trans(0, -4*UNIT, 0);

	l_u_leg.m_rot(glm::rotate(l_u_leg.m_rot(), glm::radians(22.5f), glm::vec3(1, 0, 0)));
	l_l_leg.m_rot(glm::rotate(l_l_leg.m_rot(), glm::radians(22.5f), glm::vec3(1, 0, 0)));
	r_u_leg.m_rot(glm::rotate(r_u_leg.m_rot(), glm::radians(-22.5f), glm::vec3(1, 0, 0)));
	r_l_leg.m_rot(glm::rotate(r_l_leg.m_rot(), glm::radians(-22.5f), glm::vec3(1, 0, 0)));
	l_u_arm.m_rot(glm::rotate(l_u_arm.m_rot(), glm::radians(-22.5f), glm::vec3(1, 0, 0)));
	l_l_arm.m_rot(glm::rotate(l_l_arm.m_rot(), glm::radians(-22.5f), glm::vec3(1, 0, 0)));
	r_u_arm.m_rot(glm::rotate(r_u_arm.m_rot(), glm::radians(22.5f), glm::vec3(1, 0, 0)));
	r_l_arm.m_rot(glm::rotate(r_l_arm.m_rot(), glm::radians(22.5f), glm::vec3(1, 0, 0)));
	
	// torso 
	torso.child(&head);

	// siblings (linked list; function order matters)
	head.sibling(&l_u_leg);
	l_u_leg.sibling(&r_u_leg);
	r_u_leg.sibling(&l_u_arm);
	l_u_arm.sibling(&r_u_arm);

	// childs
	l_u_leg.child(&l_l_leg);
	r_u_leg.child(&r_l_leg);
	l_u_arm.child(&l_l_arm);
	r_u_arm.child(&r_l_arm);

	this->root(&this->instances.front());
}

void scene_graph_t::animation() {
	auto& torso = this->instances[0];
	auto& head = this->instances[1];
	auto& l_u_leg = this->instances[2];
	auto& l_l_leg = this->instances[3];
	auto& r_u_leg = this->instances[4];
	auto& r_l_leg = this->instances[5];
	auto& l_u_arm = this->instances[6];
	auto& l_l_arm = this->instances[7];
	auto& r_u_arm = this->instances[8];
	auto& r_l_arm = this->instances[9];

	head.m_rot(glm::rotate(head.m_rot(), glm::radians(8.0f), glm::vec3(0, 1, 0)));

	if (0 <= this->animation_count && this->animation_count < 30) {
		l_u_leg.m_rot(glm::rotate(l_u_leg.m_rot(), glm::radians(-1.5f), glm::vec3(1, 0, 0)));
		l_l_leg.m_rot(glm::rotate(l_l_leg.m_rot(), glm::radians(-1.5f), glm::vec3(1, 0, 0)));
		r_u_leg.m_rot(glm::rotate(r_u_leg.m_rot(), glm::radians(1.5f), glm::vec3(1, 0, 0)));
		r_l_leg.m_rot(glm::rotate(r_l_leg.m_rot(), glm::radians(1.5f), glm::vec3(1, 0, 0)));
		l_u_arm.m_rot(glm::rotate(l_u_arm.m_rot(), glm::radians(1.5f), glm::vec3(1, 0, 0)));
		l_l_arm.m_rot(glm::rotate(l_l_arm.m_rot(), glm::radians(1.5f), glm::vec3(1, 0, 0)));
		r_u_arm.m_rot(glm::rotate(r_u_arm.m_rot(), glm::radians(-1.5f), glm::vec3(1, 0, 0)));
		r_l_arm.m_rot(glm::rotate(r_l_arm.m_rot(), glm::radians(-1.5f), glm::vec3(1, 0, 0)));
		this->animation_count++;
	}
	else if (30 <= this->animation_count && this->animation_count < 60) {
		l_u_leg.m_rot(glm::rotate(l_u_leg.m_rot(), glm::radians(1.5f), glm::vec3(1, 0, 0)));
		l_l_leg.m_rot(glm::rotate(l_l_leg.m_rot(), glm::radians(1.5f), glm::vec3(1, 0, 0)));
		r_u_leg.m_rot(glm::rotate(r_u_leg.m_rot(), glm::radians(-1.5f), glm::vec3(1, 0, 0)));
		r_l_leg.m_rot(glm::rotate(r_l_leg.m_rot(), glm::radians(-1.5f), glm::vec3(1, 0, 0)));
		l_u_arm.m_rot(glm::rotate(l_u_arm.m_rot(), glm::radians(-1.5f), glm::vec3(1, 0, 0)));
		l_l_arm.m_rot(glm::rotate(l_l_arm.m_rot(), glm::radians(-1.5f), glm::vec3(1, 0, 0)));
		r_u_arm.m_rot(glm::rotate(r_u_arm.m_rot(), glm::radians(1.5f), glm::vec3(1, 0, 0)));
		r_l_arm.m_rot(glm::rotate(r_l_arm.m_rot(), glm::radians(1.5f), glm::vec3(1, 0, 0)));
		this->animation_count++;
	} else {
		this->animation_count = 0;
	} 
}
