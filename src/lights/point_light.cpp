//
// Created by Jaroslav Stehlik on 23.03.2024.
//

#include "point_light.h"

glm::vec3 cg::point_light::GetColor(const ray& ray_, const glm::vec3& normal) const {

    glm::vec3 light_direction = glm::normalize(transform_.position - ray_.origin);
    float light_distance = glm::distance(ray_.origin, transform_.position);
    float light = glm::max(0.f, glm::dot(normal, light_direction));
    float attenuation = 1.0f / glm::max(0.0001f, light_distance);
    float light_normalized = glm::clamp(light * attenuation, 0.f, 1.f);
    return color_ * light_normalized;
}

void cg::point_light::SetColor(const glm::vec3 &color) {
    color_ = color;
}