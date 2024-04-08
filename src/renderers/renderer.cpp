//
// Created by Jaroslav Stehlik on 18.03.2024.
//
#include "renderer.h"

void cg::renderer::SetTransform(const transform &transform) {
    transform_ = transform;
}

const cg::transform &cg::renderer::GetTransform() const {
    return transform_;
}

bool cg::renderer::Intersects(const ray &ray, glm::vec3& intersection, glm::vec3& normal, float& raycast_distance, float max_distance) const {
    return false;
}

void cg::renderer::RecalculateBounds() {}
