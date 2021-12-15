#version 420

//TODO: discrete noise that changes based on a time uniform
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float opacity;

    sampler2D texture_diffuse1;
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

in vec2 ftex_coords;
in vec3 fposition, fnormal, flight_dir;

out vec4 output_color;

vec4 blinn_phong_specular(vec3 L, vec3 N)
{
    vec3 V = normalize(-fposition);
    vec3 R = normalize(L + V);  
    float spec = pow(max(dot(N, R), 0.f), material.shininess);

    return vec4(light.specular * (spec * material.specular), 1.f);
}

vec4 diffuse(vec3 L, vec3 N, vec4 tex_diff)
{
    float diff = max(dot(N, L), 0.f);

    return light.diffuse * (diff * tex_diff);
}

void main()
{
    // helper variables
    vec4 tex_diff = texture(material.texture_diffuse1, ftex_coords);
    float light_dist = length(flight_dir);
    vec3 L = normalize(flight_dir);
    vec3 N = normalize(fnormal);

    // attenuation
    float attK1 = 0.7f;
    float attK2 = 0.05f;
    float attK3 = 0.001f;
    float attenuation = 1.f / (attK1 +
                               attK2 * light_dist +
                               attK3 * pow(light_dist, 2));

    vec3 ambient = light.ambient * material.ambient * tex_diff;
    vec4 specular = blinn_phong_specular(L, N);
    vec4 diff_spec = diffuse(L, N, tex_diff) + specular;

    output_color = attenuation * diff_spec + ambient;
    output_color.a = material.opacity;
}
