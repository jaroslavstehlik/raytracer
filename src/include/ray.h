//
// Created by Jaroslav Stehlik on 18.03.2024.
//
#pragma once
#include <glm/vec3.hpp>

namespace cg {
    struct ray {
        glm::vec3 origin;
        glm::vec3 direction;
    };
}
