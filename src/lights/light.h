//
// Created by Jaroslav Stehlik on 23.03.2024.
//

#pragma once
#include "transform.h"
#include "ray.h"
#include "glm/glm.hpp"

namespace cg {
    class light {
    protected:
        transform transform_;
    public:
        void SetTransform(const transform &transform);
        const transform& GetTransform() const;

        [[nodiscard]] virtual glm::vec3 GetColor(const ray &ray_, const glm::vec3& normal) const;
    };
}