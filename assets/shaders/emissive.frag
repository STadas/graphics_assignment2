#version 420

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
  sampler2D texture_diffuse1;
};

uniform Material material;

out vec4 output_color;

void main()
{
  output_color = vec4(material.diffuse, 1.f);
}
