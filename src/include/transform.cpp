//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "transform.h"

glm::mat4x4 cg::transform::TranslationMatrix() const {
    return glm::translate(
            glm::mat4x4(1.0),
            glm::vec3 (position.x, position.y, position.z));
}

glm::mat4x4 cg::transform::RotationMatrix() const {
    // yaw, pitch, roll
    return glm::eulerAngleYXZ(
            glm::radians(rotation.y),
            glm::radians(rotation.x),
            glm::radians(rotation.z));
}

glm::mat4x4 cg::transform::ScaleMatrix() const {
    return glm::scale(
            glm::mat4x4(1.0),
            glm::vec3 (
                    scale.x,
                    scale.y,
                    scale.z));
}

glm::vec3 cg::transform::GetForward() const {
    return RotationMatrix() * glm::vec4(0.f, 0.f, 1.f, 0.f);
}

glm::vec3 cg::transform::GetRight() const {
    return RotationMatrix() * glm::vec4(1.f, 0.f, 0.f, 0.f);
}

glm::vec3 cg::transform::GetUp() const {
    return RotationMatrix() * glm::vec4(0.f, 1.f, 0.f, 0.f);
}

glm::mat4x4 cg::transform::ObjectToWorldMatrix() const {
    glm::mat4x4 translate_matrix = TranslationMatrix();
    glm::mat4x4 rotation_matrix = RotationMatrix();
    glm::mat4x4 scale_matrix = ScaleMatrix();
    return translate_matrix * rotation_matrix * scale_matrix;
}

glm::mat4x4 cg::transform::WorldToObjectMatrix() const {
    return glm::inverse(ObjectToWorldMatrix());
}
