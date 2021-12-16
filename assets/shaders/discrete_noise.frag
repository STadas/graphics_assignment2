#version 420

//TODO: change seed based on a time uniform
uniform float time = 1.f;

in vec2 ftex_coords;

out vec4 output_color;

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

void main()
{
    // https://thebookofshaders.com/10/
    vec2 transformed_tex_coords = floor(ftex_coords.xy * 32.f) + sin(time);
    output_color = vec4(vec3(rand(transformed_tex_coords)), 1.f);
}
