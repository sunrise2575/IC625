#ifndef __IC625_LIGHT_H__
#define __IC625_LIGHT_H__

#include "map.h"
#include "scene_graph.h"
#include "shader.h"
#include <glm/glm.hpp>

class light_t {
private:
	map_t *_map; // bind
	scene_graph_t *_target;
	glm::vec4 _Ia, _Id, _Is;
	glm::vec4 _pos, _pos_from_target;
public:
	light_t() = default;
	~light_t() = default;
	light_t(const light_t &) = delete;
	light_t(light_t &&) = delete;
	light_t& operator=(const light_t &) = delete;
	light_t& operator=(light_t &&) = delete;

	void bind_map(const map_t *);
	void place(const shader_id_t &);

	void Ia(const glm::vec4 &);
	void Id(const glm::vec4 &);
	void Is(const glm::vec4 &);
	void pos(const glm::vec4 &);
	void pos_from_target(const glm::vec4 &);

	void Ia(const float x, const float y, const float z, const float w);
	void Id(const float x, const float y, const float z, const float w);
	void Is(const float x, const float y, const float z, const float w);
	void pos(const float x, const float y, const float z, const float w);
	void pos_from_target(const float x, const float y, const float z, const float w);

	const glm::vec4 &Ia();
	const glm::vec4 &Id();
	const glm::vec4 &Is();
	const glm::vec4 &pos();
	const glm::vec4 &pos_from_target();
};

#endif