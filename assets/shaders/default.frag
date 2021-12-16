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
    vec3 position;

    vec3 ambient;
    float diffuse;
    float specular;
};

uniform Material material;
uniform Light light;

in vec3 fview_dir, fnormal, flight_pos;

out vec4 output_color;

void main()
{
    vec3 ambient = light.ambient * material.ambient * material.diffuse;
    vec3 N = normalize(fnormal);
    vec3 L = normalize(flight_pos);
    vec3 R = normalize(L + fview_dir);

    vec3 diffuse = light.diffuse * (max(dot(N, L), 0.f) * material.diffuse);
    float spec = pow(max(dot(N, R), 0.f), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    float attK1 = 0.7f;
    float attK2 = 0.05f;
    float attK3 = 0.001f;
    float attenuation = 1.f / (attK1 +
                               attK2 * length(flight_pos) +
                               attK3 * pow(length(flight_pos), 2));

    vec3 res = ambient + attenuation * (diffuse + specular);
    output_color = vec4(res, 1.f);
}
