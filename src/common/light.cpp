/** light.h
 * light struct for easy light control
 * Tadas Saltenis November 2021
 */
#include "light.h"

Light::Light(glm::vec4 position, float ambient, float diffuse, float specular)
    : position(position)
    , ambient(ambient)
    , diffuse(diffuse)
    , specular(specular)
{
}

/**
 * pass the light uniforms into the provided shader
 */
void Light::passUniforms(Shader *shader)
{
    shader->setVec4("light.position", position);
    shader->setFloat("light.ambient", ambient);
    shader->setFloat("light.diffuse", diffuse);
    shader->setFloat("light.specular", specular);
}

/**
 * pass the light uniforms into the provided shader with the view transformation
 */
void Light::passUniforms(Shader *shader, glm::mat4 &view)
{
    shader->setVec4("light.position", view * position);
    shader->setFloat("light.ambient", ambient);
    shader->setFloat("light.diffuse", diffuse);
    shader->setFloat("light.specular", specular);
}
