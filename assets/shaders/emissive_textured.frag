#version 420

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float opacity;

    sampler2D texture_diffuse1;
};

uniform Material material;

in vec2 ftex_coords;

out vec4 output_color;

void main()
{
    output_color = texture(material.texture_diffuse1, ftex_coords);
}
