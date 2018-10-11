#version 330 core

in vec3 pass_color_attr;
out vec4 frag_color;

void main() {
	frag_color = vec4(pass_color_attr, 1.0);
}
