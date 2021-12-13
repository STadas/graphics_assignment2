#version 420

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
  float opacity;

  sampler2D texture_diffuse1;
  sampler2D texture_normal1;
};

struct Light
{
  vec4 position;

  float ambient;
  float diffuse;
  float specular;
};

uniform Material material;
uniform Light light;

in vec2 ftex_coords;
in vec3 fview_dir, flight_pos, ftest_color;

out vec4 output_color;

vec4 phong_model(vec3 N, vec3 diff)
{
  vec3 ambient = light.ambient * diff;
  vec3 L = normalize(flight_pos);
  vec3 R = normalize(L + fview_dir);

  float attK1 = 0.9f;
  float attK2 = 0.005f;
  float attK3 = 0.001f;
  float attenuation = 1.f / (attK1 +
                             attK2 * length(flight_pos) +
                             attK3 * pow(length(flight_pos), 2));

  vec3 diffuse = light.diffuse * (max(dot(N, L), 0.f) * diff);
  float spec = pow(max(dot(N, R), 0.f), material.shininess);
  vec3 specular = light.specular * (spec * material.specular);

  return vec4(ambient + attenuation * (diffuse + specular), 1.f);
}

vec4 diffuse(vec3 L, vec3 N, vec4 tex_diff)
{
  float diff = max(dot(N, L), 0.f);

  return light.diffuse * (diff * tex_diff);
}

void main()
{
  vec3 tex_diff = texture(material.texture_diffuse1, ftex_coords).rgb;
  vec3 tex_normal = texture(material.texture_normal1, ftex_coords).rgb;
  vec3 normal = normalize(2.0 * tex_normal - 1.f);

  // attenuation

  output_color = phong_model(normal, tex_diff);
}

