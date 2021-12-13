/** camera.h
 * camera struct for easy camera control
 * Tadas Saltenis November 2021
 */
#include "camera.h"

OrbitCamera::OrbitCamera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : position(position)
    , target(target)
    , up(up)
    , yaw(0.f)
    , pitch(0.f)
    , radius(2.f)
{
}
