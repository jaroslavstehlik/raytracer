//
// Created by Jaroslav Stehlik on 24.03.2024.
//
#pragma once
#include "renderer.h"

namespace cg {
    class plane_renderer : public renderer {

    public:
        [[nodiscard]] bool Intersects(const ray &ray,
                                      glm::vec3 &intersection,
                                      glm::vec3 &normal,
                                      float& raycast_distance,
                                      glm::vec3& barycentric_coords,
                                      glm::vec2& uv,
                                      float max_distance) const override;
        virtual void Prepass() override;
    };
}
