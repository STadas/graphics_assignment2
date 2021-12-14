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
	gl_Position = transform.model * vec4(position, 1.f);
}
