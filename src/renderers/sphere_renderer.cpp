//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "sphere_renderer.h"
#include "algorithm"

void sphere_renderer::SetRadius(float radius) {
    radius_ = radius;
}

bool sphere_renderer::Intersects(const ray &ray, float raycast_distance) const {
    // closest point on line
    // radius to closest point on line

    glm::vec3 localPosition = transform_.position - ray.origin;
    glm::vec3 closestPoint = ray.origin + ray.direction * std::clamp(glm::dot(ray.direction, localPosition), 0.f, raycast_distance);

    float distance = glm::distance(transform_.position, closestPoint);
    return distance <= radius_;
}
