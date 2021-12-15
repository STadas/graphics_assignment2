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
    // get vector between fragment position and light position
    vec3 to_light = ffrag_pos - light.position;
    // use the fragment to light vector to sample from the depth map    
    float closest_depth = texture(material.cube_map1, to_light).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closest_depth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float curr_depth = length(to_light);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = curr_depth - bias > closest_depth ? 1.0 : 0.0;        

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

    vec3 res = attenuation * (ambient + (1.f - shadow()) * (diffuse + specular));
    output_color = vec4(res, 1.f);
}
