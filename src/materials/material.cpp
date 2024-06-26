//
// Created by Jaroslav Stehlik on 14.04.2024.
//

#include "material.h"
namespace cg {
    material::material(const std::shared_ptr<cg::texture>& albedo_texture,
                       const glm::vec4& albedo_color,
                       const glm::vec3& emission_color,
                       float metallic,
                       float roughness)
        : albedo_texture(albedo_texture)
        , albedo_color(albedo_color)
        , emission_color(emission_color)
        , metallic(metallic)
        , roughness(roughness) {}
}