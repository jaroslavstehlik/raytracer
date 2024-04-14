//
// Created by Jaroslav Stehlik on 14.04.2024.
//

#pragma once
#include "glm/glm.hpp"

namespace cg {
    class material {
    public:
        glm::vec4 albedo;
        float roughness;
        float metallic;
    };
}