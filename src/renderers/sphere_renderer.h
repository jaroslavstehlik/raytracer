//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#pragma once
#include "renderer.h"

namespace cg {
    class sphere_renderer : public renderer {
    private:
        float radius_ = 1.f;

    public:
        void SetRadius(float radius);

        [[nodiscard]] bool Intersects(const ray &ray,
                                      glm::vec3 &intersection,
                                      glm::vec3 &normal,
                                      float& raycast_distance,
                                      glm::vec3& barycentric_coords,
                                      glm::vec2& uv,
                                      float max_distance) const override;
        void Prepass() override;
    };
}
