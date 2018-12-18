#ifndef __IC625_OBJECT_LOADER_H__ 
#define __IC625_OBJECT_LOADER_H__ 

#include <vector>
#include <glm/glm.hpp>

bool load_object(
	const char *path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals);

bool load_KaKdKsIllum(const char *path, glm::vec4 &Ka, glm::vec4 &Kd, glm::vec4 &Ks, float &illum);

#endif