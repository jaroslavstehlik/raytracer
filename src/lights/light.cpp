//
// Created by Jaroslav Stehlik on 23.03.2024.
//

#include "light.h"

void cg::light::SetTransform(const cg::transform &transform) {
    transform_ = transform;
}

const cg::transform &cg::light::GetTransform() const {
    return transform_;
}

glm::vec3 cg::light::GetColor(const ray& ray_, const glm::vec3& normal) const {
    return glm::vec3();
}
