/** light.h
 * light struct for easy light control
 * Tadas Saltenis November 2021
 */
#include "light.h"

Light::Light(glm::vec3 position, glm::vec3 ambient, float diffuse, float specular)
    : position(position)
    , ambient(ambient)
    , diffuse(diffuse)
    , specular(specular)
{
}
