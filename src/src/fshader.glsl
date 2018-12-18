#version 330 core

in struct {
	vec3 frag_pos;
	vec2 texture;
	vec3 normal;
	vec3 light;
	float fog;
} fin;

uniform struct {
	sampler2D texture, normap;
} sampler;

uniform struct {
	vec4 ka, kd, ks;
	vec4 ia, id, is;
	float shininess;
} phong;

uniform struct {
	vec4 light;
	vec3 eye;
} pos;

out vec4 color;

void main() {
	vec3 NORMAL = normalize(fin.normal + (2 * vec3(texture(sampler.normap, fin.texture)) - 1));
	vec3 LIGHT = normalize(vec3(pos.light) - fin.frag_pos);
	vec3 VIEW = normalize(vec3(pos.eye) - fin.frag_pos);
	vec3 REFLECT = reflect(-LIGHT, NORMAL);
	
	vec3 AMBIENT = vec3(phong.ka * phong.ia);
	vec3 DIFFUSE = vec3(phong.kd * phong.id * max(dot(NORMAL, LIGHT), 0.0));
	vec3 SPECULAR = vec3(phong.ks * phong.is) * pow(max(dot(VIEW, REFLECT), 0.0), phong.shininess);

	vec3 RESULT = (AMBIENT + DIFFUSE + SPECULAR) * vec3(texture(sampler.texture, fin.texture)) * fin.fog;
	
	color = vec4(RESULT, 1.0);
}