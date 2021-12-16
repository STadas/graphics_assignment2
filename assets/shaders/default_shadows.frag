#version 420

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float opacity;

    samplerCube cube_map1;
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
uniform float far_plane;

in vec3 ffrag_pos, fview_dir, fnormal, flight_pos;

out vec4 output_color;

float shadow()
{
    vec3 to_light = ffrag_pos - light.position;
    float curr_depth = length(to_light);
    float shadow = 0.f;
    float bias = 0.05; 
    float samples = 4.f;
    float offs = 0.1;
    for(float x = -offs; x < offs; x += offs / (samples * 0.5))
    {
        for(float y = -offs; y < offs; y += offs / (samples * 0.5))
        {
            for(float z = -offs; z < offs; z += offs / (samples * 0.5))
            {
                float closest_depth = texture(material.cube_map1, to_light + vec3(x, y, z)).r; 
                closest_depth *= far_plane;   // undo mapping [0;1]
                if(curr_depth - bias > closest_depth)
                    shadow += 1.f;
            }
        }
    }
    shadow /= pow(samples, 3);

    return shadow;
}

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

    vec3 res = ambient + attenuation * (1.f - shadow()) * (diffuse + specular);
    output_color = vec4(res, 1.f);
}
