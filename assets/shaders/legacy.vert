#version 420

struct Transform
{
  mat3 m3_normal;
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

// uniform variables
uniform Transform transform;
uniform Light light;

// vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

// output variables
out vec3 fposition, fnormal, flight_dir;

void main()
{
  vec4 h_position = vec4(position, 1.f);
  mat4 mv_matrix = transform.view * transform.model;

  fposition = (mv_matrix * h_position).xyz;
  fnormal = normalize(transform.m3_normal * normal);
  flight_dir = light.position.xyz - fposition;

  gl_Position = (transform.projection * mv_matrix) * h_position;
}
