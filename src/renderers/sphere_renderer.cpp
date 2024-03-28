//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "sphere_renderer.h"
#include "algorithm"

void cg::sphere_renderer::SetRadius(float radius) {
    radius_ = radius;
}

bool cg::sphere_renderer::Intersects(const ray &ray, glm::vec3& intersection, glm::vec3& normal, float& raycast_distance, float max_distance) const {
    // Analytical

    float raySphereDotProduct = glm::dot(ray.direction, transform_.position - ray.origin);
    if (raySphereDotProduct < 0.f)
        return false;

    glm::vec3 projectedPoint = ray.origin + ray.direction * raySphereDotProduct;
    glm::vec3 d = transform_.position - projectedPoint;
    float distanceSqr = glm::dot(d, d);

    float radius2 = radius_ * radius_;
    if (radius2 <= distanceSqr)
        return false;

    float intersectionDistance = sqrt(radius2 - distanceSqr);
    intersection = projectedPoint - ray.direction * intersectionDistance;
    glm::vec3 furthestIntersection = projectedPoint + ray.direction * intersectionDistance;
    raycast_distance = raySphereDotProduct - intersectionDistance;

    if(raycast_distance < 0.f ||
        raycast_distance > max_distance)
        return false;

    normal = glm::normalize(intersection - transform_.position);
    return true;
}
