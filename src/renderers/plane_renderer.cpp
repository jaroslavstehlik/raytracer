//
// Created by Jaroslav Stehlik on 24.03.2024.
//

#include "plane_renderer.h"

bool cg::plane_renderer::Intersects(const cg::ray &ray, glm::vec3 &intersection, glm::vec3 &normal, float &raycast_distance,
                               float max_distance) const {

    normal = transform_.GetUp();
    float epsilon = 1e-6;
    float dot_product = glm::dot(ray.direction, normal);
    if(abs(dot_product) < epsilon)
        return false;

    glm::vec3 local_position = transform_.position - ray.origin;
    raycast_distance = glm::dot(local_position, normal) / dot_product;
    if(raycast_distance < 0.f)
        return false;

    // apply shadow bias to prevent shadow acne
    raycast_distance -= epsilon;

    intersection = ray.origin + ray.direction * raycast_distance;
    return true;
}
