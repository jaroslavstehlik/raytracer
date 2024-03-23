//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "transform.h"

glm::mat4x4 cg::transform::ObjectToWorldMatrix() const {
    glm::mat4x4 translate_matrix = glm::translate(
            glm::mat4x4(1.0),
            glm::vec3 (position.x, position.y, position.z));

    // yaw, pitch, roll
    glm::mat4x4 rotation_matrix = glm::eulerAngleYXZ(
            glm::radians(rotation.y),
            glm::radians(rotation.x),
            glm::radians(rotation.z));

    glm::mat4x4 scale_matrix = glm::scale(
            glm::mat4x4(1.0),
            glm::vec3 (scale.x, scale.y, scale.z));

    return scale_matrix * rotation_matrix * translate_matrix;
}
