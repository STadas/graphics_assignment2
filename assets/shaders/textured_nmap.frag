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
    vec3 position;

    vec3 ambient;
    float diffuse;
    float specular;
};

uniform Material material;
uniform Light light;

in vec2 ftex_coords;
in vec3 fview_dir, flight_pos;

out vec4 output_color;

void main()
{
    vec3 tex_diff = texture(material.texture_diffuse1, ftex_coords).rgb;
    /* tex_diff = vec3(0.5f, 0.5f, 0.5f); */
    vec3 tex_normal = texture(material.texture_normal1, ftex_coords).rgb;

    vec3 ambient = light.ambient * material.ambient * tex_diff;
    vec3 N = normalize(2.0 * tex_normal - 1.f);
    vec3 L = normalize(flight_pos);
    vec3 R = normalize(L + fview_dir);

    vec3 diffuse = light.diffuse * (max(dot(N, L), 0.f) * tex_diff);
    float spec = pow(max(dot(N, R), 0.f), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    float attK1 = 0.7f;
    float attK2 = 0.05f;
    float attK3 = 0.001f;
    float attenuation = 1.f / (attK1 +
                               attK2 * length(flight_pos) +
                               attK3 * pow(length(flight_pos), 2));

    vec3 res = attenuation * (ambient + (diffuse + specular));
    output_color = vec4(res, 1.f);
}

