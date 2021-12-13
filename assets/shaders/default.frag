#version 420

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
  float opacity;
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

in vec3 fposition, fnormal, flight_dir;

out vec4 output_color;

vec3 blinn_phong_specular(vec3 L, vec3 N)
{
  vec3 V = normalize(-fposition);
  vec3 R = normalize(L + V);  
  float spec = pow(max(dot(N, R), 0.f), material.shininess);

  return light.specular * (spec * material.specular);
}

vec3 diffuse(vec3 L, vec3 N)
{
  float diff = max(dot(N, L), 0.f);

  return light.diffuse * (diff * material.diffuse);
}

void main()
{
  // helper variables
  float light_dist = length(flight_dir);
  vec3 L = normalize(flight_dir);
  vec3 N = normalize(fnormal);

  // attenuation
  float attK1 = 0.8f;
  float attK2 = 0.05f;
  float attK3 = 0.0001f;
  float attenuation = 1.f / (attK1 +
                             attK2 * light_dist +
                             attK3 * pow(light_dist, 2));

  vec3 ambient = material.diffuse * material.ambient * light.ambient;
  vec3 specular = blinn_phong_specular(L, N);
  vec3 diff_spec = diffuse(L, N) + specular;

  output_color = vec4(attenuation * diff_spec + ambient, material.opacity);
}
