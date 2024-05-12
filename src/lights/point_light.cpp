//
// Created by Jaroslav Stehlik on 23.03.2024.
//

#include "point_light.h"

glm::vec4 cg::point_light::GetColor(const ray& ray_, const glm::vec3& normal) const {
    glm::vec3 light_direction = glm::normalize(transform_.position - ray_.origin);
    float light_distance = glm::distance(ray_.origin, transform_.position);
    float light = glm::max(0.f, glm::dot(normal, light_direction));
    // https://en.wikipedia.org/wiki/Inverse-square_law
    float attenuation = 1.0f / glm::max(std::numeric_limits<float>::min(), light_distance * light_distance);
    return color_ * light * attenuation;
}

void cg::point_light::SetColor(const glm::vec4 &color) {
    color_ = color;
}