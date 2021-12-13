#version 420

struct Transform
{
	mat4 model;
	mat4 view;
	mat4 projection;
};

uniform Transform transform;

layout(location = 0) in vec3 position;

void main()
{
	vec4 h_position = vec4(position, 1.f);
	mat4 mv_matrix = transform.view * transform.model;
	gl_Position = (transform.projection * mv_matrix) * h_position;
}
