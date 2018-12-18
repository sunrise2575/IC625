#ifndef __IC625_SHADER_H__ 
#define __IC625_SHADER_H__ 

struct shader_id_t {
	GLuint shader;
	struct { GLuint vertex, texture, normal, tangent, bitangent; } coord;
	struct { GLuint proj, view, scale, rot, trans; } mat;
	struct { GLuint texture, normap; } sampler;
	struct { GLuint light, eye; } pos;
	struct { GLuint ka, kd, ks, ia, id, is, shininess; } phong;
	
	void load_shader(const char * vertex_file_path, const char * fragment_file_path);
};

#endif