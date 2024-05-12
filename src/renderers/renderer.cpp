//
// Created by Jaroslav Stehlik on 18.03.2024.
//
#include "renderer.h"

void cg::renderer::SetTransform(const cg::transform &transform) {
    transform_ = transform;
}

const cg::transform &cg::renderer::GetTransform() const {
    return transform_;
}

void cg::renderer::SetMaterial(const std::shared_ptr<cg::material>& material) {
    material_ = material;
}

const std::shared_ptr<cg::material>& cg::renderer::GetMaterial() const {
    return material_;
}

bool cg::renderer::Intersects(const ray &ray,
                              glm::vec3& intersection,
                              glm::vec3& normal,
                              float& raycast_distance,
                              glm::vec3& barycentric_coords,
                              glm::vec2& uv,
                              float max_distance) const {
    return false;
}

void cg::renderer::Prepass() {}
