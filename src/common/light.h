/** light.h
 * light struct for easy light control
 * Tadas Saltenis November 2021
 */
#pragma once

#include <glm/glm.hpp>
#include "shader.h"

struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    float diffuse;
    float specular;

    Light(glm::vec3 position, glm::vec3 ambient = glm::vec3(0.1f), float diffuse = 1.0,
          float specular = 1.0);
};
