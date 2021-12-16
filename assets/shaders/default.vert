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
	vec3 position;

	vec3 ambient;
	float diffuse;
	float specular;
};

uniform Transform transform;
uniform Light light;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coords;

out vec2 ftex_coords;
out vec3 fview_dir, fnormal, flight_pos;

void main()
{
	vec4 h_position = vec4(position, 1.f);
	vec4 h_lightpos = vec4(light.position, 1.f);
	mat4 mv_matrix = transform.view * transform.model;
	mat3 normalmatrix = mat3(transpose(inverse(mv_matrix)));

	fview_dir = normalize(-vec3((mv_matrix * h_position)));
	flight_pos = vec3(transform.view * h_lightpos - (mv_matrix * h_position));
	fnormal = normalize(normalmatrix * normal);
	ftex_coords = tex_coords;

	gl_Position = (transform.projection * mv_matrix) * h_position;
}
