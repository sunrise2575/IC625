#version 330 core

in struct {
	vec3 vertex;
	vec2 texture;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} coord;

uniform struct {
	mat4 proj, view, scale, rot, trans;
} mat;


out struct {
	vec3 frag_pos;
	vec2 texture;
	vec3 normal;
	vec3 light;
	float fog;
} fin;

float fog_linear(const float distance, const float fog_start, const float fog_end) {
	return clamp((fog_end - distance) / (fog_end - fog_start), 0.0, 1.0);
}

void main() {
	mat4 MODEL = mat.trans * mat.rot * mat.scale;
	
	gl_Position = mat.proj * mat.view * MODEL * vec4(coord.vertex, 1.0);
	fin.frag_pos = vec3(MODEL * vec4(coord.vertex, 1.0));
	fin.texture = coord.texture;
	fin.normal = mat3(transpose(inverse(MODEL))) * coord.normal;

	fin.fog = fog_linear(length(vec3(gl_Position)), 0, 30);
}