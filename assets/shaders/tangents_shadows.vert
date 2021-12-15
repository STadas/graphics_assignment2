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
layout(location = 3) in vec3 tangent;

out vec2 ftex_coords;
out vec3 ffrag_pos, fview_dir, flight_pos;
out mat3 toObjectLocal;

void main()
{
	vec4 h_position = vec4(position, 1.f);
	vec4 h_tangent = vec4(tangent, 1.f);
	vec4 h_lightpos = vec4(light.position, 1.f);

	mat4 mv_matrix = transform.view * transform.model;
	mat3 normalmatrix = mat3(transpose(inverse(mv_matrix)));

	vec3 norm = normalize(normalmatrix * normal);
	vec3 tang = normalize(normalmatrix * tangent);
	vec3 bitang = normalize(cross(norm, tang)) * h_tangent.w;

	mat3 toObjectLocal = mat3(tang.x, bitang.x, norm.x,
							  tang.y, bitang.y, norm.y,
							  tang.z, bitang.z, norm.z);

	ffrag_pos = vec3(transform.model * h_position);

	flight_pos = toObjectLocal * vec3(transform.view * h_lightpos - (mv_matrix * h_position));
	fview_dir = normalize(toObjectLocal * normalize(-vec3(mv_matrix * h_position)));
	ftex_coords = tex_coords;

	gl_Position = (transform.projection * mv_matrix) * h_position;
}
