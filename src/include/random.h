//
// Created by Jaroslav Stehlik on 24.05.2024.
//

#pragma once
#include <cstdlib>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"

namespace cg {
    inline float RandomFloat() {
        return (float)std::rand() / (float)RAND_MAX;
    }

    inline float RandomFloat(float min, float max) {
        return min + RandomFloat() * (max - min);
    }

    // TODO: this is inefficient due to normalization
    inline glm::vec3 RandomDirection() {
        return glm::normalize(glm::vec3 {
            RandomFloat(-1.f, 1.f), RandomFloat(-1.f, 1.f), RandomFloat(-1.f, 1.f)
        });
    }

    // TODO: this is inefficient due to square root and normalization
    inline glm::vec3 RandomPositionInsideUnitSphere() {
        return RandomDirection() * sqrt(RandomFloat());
    }

    // TODO: this is inefficient due to square root and normalization
    inline glm::vec2 RandomPositionInsideUnitDisc() {
        return glm::normalize(glm::vec2{RandomFloat(-1.f, 1.f), RandomFloat(-1.f, 1.f)}) * sqrt(RandomFloat());
    }
}