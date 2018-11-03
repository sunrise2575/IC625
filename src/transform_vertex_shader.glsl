#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex_in; // VBO location 0
layout(location = 1) in vec2 texcoord_in; // VBO location 1
//layout(location = 2) in vec3 normal_in; // VBO location 2

// Output data ; will be interpolated for each fragment.
out vec2 texcoord_out;

// Values that stay constant for the whole mesh.
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 scale_mat;
uniform mat4 rot_mat;
uniform mat4 trans_mat;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	vec4 temp = vec4(vertex_in, 1.0);
	temp = scale_mat * temp;
	temp = rot_mat * temp;
	temp = trans_mat * temp;

	gl_Position = proj_mat * view_mat * model_mat * temp;
	texcoord_out = texcoord_in;
}