//
// Created by Jaroslav Stehlik on 08.05.2024.
//
#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace cg {
    struct intersection_result {
        glm::vec3 intersection;
        glm::vec3 normal;
        glm::vec2 uv;
        float raycast_distance;
    };
}