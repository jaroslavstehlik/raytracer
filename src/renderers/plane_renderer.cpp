//
// Created by Jaroslav Stehlik on 24.03.2024.
//

#include "plane_renderer.h"
#include "global_settings.h"

bool cg::plane_renderer::Intersects(const cg::ray &ray,
                                    glm::vec3 &intersection,
                                    glm::vec3 &normal,
                                    float &raycast_distance,
                                    glm::vec3& barycentric_coords,
                                    glm::vec2& uv,
                                    float max_distance) const {

    normal = transform_.GetUp();
    float dot_product = glm::dot(ray.direction, normal);
    if(abs(dot_product) < cg::kEpsilon)
        return false;

    glm::vec3 local_position = transform_.position - ray.origin;
    raycast_distance = glm::dot(local_position, normal) / dot_product;
    if(raycast_distance < 0.f)
        return false;

    // apply shadow bias to prevent shadow acne
    raycast_distance -= cg::kEpsilon;

    intersection = ray.origin + ray.direction * raycast_distance;
    return true;
}

void cg::plane_renderer::Prepass() {
    bounds_.min = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    bounds_.max = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
}
