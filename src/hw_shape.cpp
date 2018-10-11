#include "common.h"

namespace hw::shape {
	hw::shape_t triangle_vertex, triangle_color;
	hw::shape_t square_vertex, square_color;
	hw::shape_t tree_vertex, tree_color;
	hw::shape_t car_vertex, car_color;
}

void hw::set_shape() {
	hw::shape::triangle_vertex.arr = std::vector<glm::vec3>{
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f)
	};
	hw::shape::triangle_color.arr = std::vector<glm::vec3>{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};

	hw::shape::square_vertex.arr = std::vector<glm::vec3>{
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f)
	};
	hw::shape::square_color.arr = std::vector<glm::vec3>{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};

	hw::shape::tree_vertex.arr = std::vector<glm::vec3>{
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f)
	};
	hw::shape::tree_color.arr = std::vector<glm::vec3>{
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	hw::shape::car_vertex.arr = std::vector<glm::vec3>{
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f)
	};
	hw::shape::car_color.arr = std::vector<glm::vec3>{
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};

}
