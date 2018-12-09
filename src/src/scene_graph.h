#include <stack>
#include <glm/glm.hpp>
#include "instance.h"

class scene_graph_elem_t : public instance_t {
protected:
	scene_graph_elem_t *_child, *_sibling;
public:
	scene_graph_elem_t *child();
	scene_graph_elem_t *sibling();
	void child(scene_graph_elem_t *elem);
	void sibling(scene_graph_elem_t *elem);
	void draw(const asset_kv_t &assets, const shader_id_t &shader_id) = delete;
	void draw(const glm::mat4 &matrix, const asset_kv_t &assets, const shader_id_t &shader_id);
};

class scene_graph_t {
private:
	scene_graph_elem_t *_root;
	std::stack<glm::mat4> stack;
	std::vector<scene_graph_elem_t> instances;

	void _draw_recursive(scene_graph_elem_t *node, const asset_kv_t &assets, const shader_id_t &shader_id);
public:
	scene_graph_t() { this->_root = nullptr; }
	scene_graph_t(const scene_graph_elem_t &) = delete;
	scene_graph_t(scene_graph_elem_t &&) = delete;
	~scene_graph_t() {}
	scene_graph_elem_t &operator=(const scene_graph_elem_t &) = delete;
	scene_graph_elem_t &operator=(scene_graph_elem_t &&) = delete;

	void root(const scene_graph_elem_t *root);
	void draw(const asset_kv_t &assets, const shader_id_t &shader_id);
	void create_humanoid();
};
