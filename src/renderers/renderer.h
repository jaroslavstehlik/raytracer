//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#pragma once
#include "ray.h"
#include "transform.h"
#include "AABB.h"
#include "../materials/material.h"

namespace cg {
    class renderer {
    protected:
        cg::transform transform_;
        cg::AABB bounds_;
        std::shared_ptr<material> material_;

    public:
        void SetTransform(const cg::transform &transform);
        const cg::transform& GetTransform() const;

        void SetMaterial(const std::shared_ptr<cg::material>& material);
        const std::shared_ptr<cg::material>& GetMaterial() const;

        [[nodiscard]] virtual bool
        Intersects(const ray &ray,
                   glm::vec3 &intersection,
                   glm::vec3 &normal,
                   float& raycast_distance,
                   glm::vec2& uv,
                   float max_distance) const;
        virtual void Prepass();
    };
}
