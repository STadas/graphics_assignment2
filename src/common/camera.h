/** camera.h
 * camera struct for easy camera control
 * Tadas Saltenis November 2021
 */
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct OrbitCamera {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    GLfloat yaw;
    GLfloat pitch;

    glm::mat4 projection;
    glm::mat4 view;

    OrbitCamera(glm::vec3 position, glm::vec3 target = glm::vec3(0, 0, 0),
           glm::vec3 up = glm::vec3(0, 1, 0));
};
