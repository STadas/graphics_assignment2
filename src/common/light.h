/** light.h
 * light struct for easy light control
 * Tadas Saltenis November 2021
 */
#pragma once

#include <glm/glm.hpp>
#include "shader.h"

struct Light {
    glm::vec4 position;
    float ambient;
    float diffuse;
    float specular;

    Light(glm::vec4 position, float ambient = 0.1, float diffuse = 1.0,
          float specular = 1.0);
};
