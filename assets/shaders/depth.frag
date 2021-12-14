#version 420

uniform vec3 light_pos;
uniform float far_plane;

in vec4 frag_pos;

void main()
{
	float light_dist = length(frag_pos.xyz - light_pos);
	light_dist = light_dist / far_plane;
	gl_FragDepth = light_dist;
}
