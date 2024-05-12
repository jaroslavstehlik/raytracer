//
// Created by Jaroslav Stehlik on 14.04.2024.
//

#pragma once
#include <string>
#include "glm/glm.hpp"
#include "texture.h"

namespace cg {
    class material {
    public:
        material(const std::shared_ptr<cg::texture>& albedo_texture, const glm::vec4& albedo_color, float metallic, float roughness);
        std::shared_ptr<cg::texture> albedo_texture;
        glm::vec4 albedo_color;
        float metallic;
        float roughness;
    };
}