// Minimal vertex shader
#version 420


struct Transform
{
	mat4 model;
	mat4 view;
	mat4 projection;
};

struct Light
{
	vec4 position;

	float ambient;
	float diffuse;
	float specular;
};

uniform Transform transform;
uniform Light light;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coords;

out vec2 ftex_coords;
out vec3 ffrag_pos, fview_dir, fnormal, flight_pos;

void main()
{
	vec4 h_position = vec4(position, 1.f);
	mat4 mv_matrix = transform.view * transform.model;
	mat3 normalmatrix = mat3(transpose(inverse(mv_matrix)));

	ffrag_pos = vec3(transform.model * h_position);
	fview_dir = (mv_matrix * h_position).xyz;
	fnormal = normalize(normalmatrix * normal);
	flight_pos = (transform.view * light.position - (mv_matrix * h_position)).xyz;
	ftex_coords = tex_coords;

	gl_Position = (transform.projection * mv_matrix) * h_position;
}
