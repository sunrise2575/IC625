#ifndef __IC625_SHADER_H__ 
#define __IC625_SHADER_H__ 

struct shader_id_t {
	GLuint shader;
	GLuint model_mat, view_mat, proj_mat;
	GLuint scale_mat, rot_mat, trans_mat;
	GLuint tex_sampler;
	
	void load_shader(const char * vertex_file_path, const char * fragment_file_path);
};

#endif