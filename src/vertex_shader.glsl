#version 330 core

in vec3 vertex_attr;
in vec3 color_attr;

out vec3 pass_color_attr;

uniform mat4 transform_mat;
uniform mat4 model_view_proj;

void main() {
	gl_Position = model_view_proj * transform_mat * vec4(vertex_attr, 1.0f);
	pass_color_attr = color_attr;
}
