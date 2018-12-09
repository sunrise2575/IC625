#include "scene_graph.h"

scene_graph_elem_t *scene_graph_elem_t::child() {
	return this->_child;
}

scene_graph_elem_t *scene_graph_elem_t::sibling() {
	return this->_sibling;
}

void scene_graph_elem_t::child(scene_graph_elem_t *elem) {
	this->_child = elem;
}

void scene_graph_elem_t::sibling(scene_graph_elem_t *elem) {
	this->_sibling = elem;
}

void scene_graph_elem_t::draw(
	const glm::mat4 &matrix, const asset_kv_t &assets, const shader_id_t &shader_id)
{
		const auto& data = assets.find(this->_asset)->second;
		
		// Apply Transform
		glUniformMatrix4fv(shader_id.scale_mat, 1, GL_FALSE, &matrix[0][0]);
		glUniformMatrix4fv(shader_id.rot_mat, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
		glUniformMatrix4fv(shader_id.trans_mat, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);

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

void scene_graph_t::root(const scene_graph_elem_t *root) {
	this->_root = const_cast<scene_graph_elem_t *>(root);
}

void scene_graph_t::_draw_recursive(scene_graph_elem_t *node, const asset_kv_t &assets, const shader_id_t &shader_id) {
	// pre-order tree traversal
	if (!node) return;
	this->stack.push(this->stack.top()); // PushMatrix
	this->stack.top() *= node->m_trans() * node->m_rot() * node->m_scale(); // MulMatrix
	node->draw(this->stack.top(), assets, shader_id); // Draw
	if (node->child())
		this->_draw_recursive(node->child(), assets, shader_id);
	this->stack.pop();
	if (node->sibling())
		this->_draw_recursive(node->sibling(), assets, shader_id);
}

void scene_graph_t::draw(const asset_kv_t &assets, const shader_id_t &shader_id) {
	this->stack.push(glm::mat4(1.0));
	this->_draw_recursive(this->_root, assets, shader_id);
}

void scene_graph_t::create_humanoid() {
	scene_graph_elem_t torso, head;
	scene_graph_elem_t l_u_leg, l_l_leg, r_u_leg, r_l_leg;
	scene_graph_elem_t l_u_arm, l_l_arm, r_u_arm, r_l_arm;
	
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

	// add all to instances for preserving
	this->instances.push_back(torso);
	this->instances.push_back(head);

	this->instances.push_back(l_u_leg);
	this->instances.push_back(l_l_leg);
	this->instances.push_back(r_u_leg);
	this->instances.push_back(r_l_leg);

	this->instances.push_back(l_u_arm);
	this->instances.push_back(l_l_arm);
	this->instances.push_back(r_u_arm);
	this->instances.push_back(r_l_arm);
}
