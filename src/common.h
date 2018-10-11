#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <random>
#include <typeinfo>
#include <mutex>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hw {
	constexpr GLuint GRIDS = 10;

	GLuint load_shader(const char* vertex_file_path, const char* fragment_file_path);
	void set_shape();

	struct shape_t;

	class object_t;
		class player_t; class car_t; class tree_t;
	
	class tile_line_t;
		class road_t; class ground_t;

	class map_t;

	enum class tile_line_type { ground, road };

	extern GLFWwindow* window;
	extern GLuint CAMERA_WIDTH, CAMERA_HEIGHT;
	extern GLuint GRID_WIDTH;
	extern player_t player;
	extern std::list<hw::tile_line_t*> map;
	extern std::mutex map_mtx;
}

struct hw::shape_t {
	GLuint vbo_idx;
	std::vector<glm::vec3> arr;
};

class hw::object_t {
protected:
	hw::shape_t vertex, color;
	glm::mat4 mat_trans, mat_rot, mat_scale;
	glm::vec3 position;
	GLuint VAO, SHADER;
	void __make_vbo(hw::shape_t&);
	void __remove_vbo(const GLuint vbo_idx);
	void __place(const glm::vec3& source);
public:
	object_t();
	~object_t();

	void draw();

	// For class init
	void set_vertex(const hw::shape_t&);
	void set_color(const hw::shape_t&);
	void set_VAO(const GLuint VAO);
	void set_shader(const GLuint shader_pid);
	void bind_vbo_to_vao();

	// For placing
	void place_on_grid(const GLint grid_x, const GLint grid_y);
	void place(const GLint x, const GLint y);
	void scale(const GLfloat x, const GLfloat y);

	glm::mat4 get_mat();
};

class hw::player_t : public hw::object_t {
	GLint grid_x, grid_y;
private:
	glm::mat4 mat_proj, mat_view, mat_model;
	glm::vec3 view_at, view_to, view_up;
public:
	std::list<hw::tile_line_t*>::iterator map_iter;
	void key_input(int key, int action);

	void set_mat_proj(glm::mat4 mat);
	void set_mat_view(glm::mat4 mat);
	void set_view(const glm::vec3 at, const glm::vec3 to, const glm::vec3 up);
	void set_mat_model(glm::mat4 mat);

	void move_on_grid(const GLint grid_dx, const GLint grid_dy);
	void place_on_grid(const GLint grid_x, const GLint grid_y);

	glm::vec2 get_grid();
	glm::mat4 get_camera();

	void move_camera(const GLfloat dx, const GLfloat dy);
	void check_dead();
	void set_starting_line();
};

class hw::car_t : public hw::object_t {
private:
	GLfloat speed;
public:
	void set_speed(const GLfloat speed);
	glm::vec3 get_pos();
	void move();
};

class hw::tree_t : public hw::object_t {
private:
	GLint grid_x, grid_y;
public:
	glm::vec2 get_grid();
	void place_on_grid(const GLint grid_x, const GLint grid_y);
};

class hw::tile_line_t {
protected:
	int index;
public:
	virtual ~tile_line_t() {}

	virtual int get_index() = 0;
	virtual void draw() = 0;
	virtual hw::tile_line_type type() = 0;
	virtual void gen(const GLuint shader_pid, const GLuint VAO) = 0;
	virtual void move() = 0;
	virtual void remove() = 0;
};

class hw::road_t : public hw::tile_line_t {
private:
	bool is_plus_y_direction;
	GLfloat lane_speed;
public:
	std::list<hw::car_t> elements;
	road_t(const int index);
	~road_t() {}

	int get_index();
	void draw();
	hw::tile_line_type type();
	void gen(const GLuint shader_pid, const GLuint VAO);
	void remove();
	void move();
};

class hw::ground_t : public hw::tile_line_t {
private:
	bool is_generated;
public:
	std::list<hw::tree_t> elements;
	ground_t(const int index);
	~ground_t() {}

	int get_index();
	void draw();
	hw::tile_line_type type();
	void gen(const GLuint shader_pid, const GLuint VAO);
	void remove();
	void move();
};

namespace hw::random {
	int uniform(const int min, const int max);
	float uniform(const float min, const float max);
	bool binary(const float p);
}
namespace hw::shape {
	extern hw::shape_t triangle_vertex, triangle_color;
	extern hw::shape_t square_vertex, square_color;
	extern hw::shape_t tree_vertex, tree_color;
	extern hw::shape_t car_vertex, car_color;
}

namespace hw::glfw {
	void init();
	void quit(const char* msg); 
	void version(const float version);
}

namespace hw::glew {
	void init();
	void quit(const char* msg);
}

namespace hw::glfw::callback {
	void error(int error_code, const char* error_description);
	void key(GLFWwindow* window, int key, int scancode, int action, int mods);
	void resize(GLFWwindow* window, int width, int height);
}
