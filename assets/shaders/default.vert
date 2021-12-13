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
out vec3 fposition, fnormal, flight_dir;

void main()
{
	vec4 h_position = vec4(position, 1.f);
	mat4 mv_matrix = transform.view * transform.model;
	mat3 normalmatrix = mat3(transpose(inverse(mv_matrix)));

	fposition = (mv_matrix * h_position).xyz;
	fnormal = normalize(normalmatrix * normal);
	flight_dir = light.position.xyz - fposition;
	ftex_coords = tex_coords;

	gl_Position = (transform.projection * mv_matrix) * h_position;
}