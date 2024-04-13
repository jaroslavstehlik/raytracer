//
// Created by Jaroslav Stehlik on 08.04.2024.
//

#pragma once
#include <span>
#include "glm/glm.hpp"
#include "ray.h"

namespace cg {
    class AABB {
    public:
        glm::vec3 min;
        glm::vec3 max;

        bool Intersects(const cg::ray &ray, float raycast_distance) const {
            float tx1 = (min.x - ray.origin.x) / ray.direction.x, tx2 = (max.x - ray.origin.x) / ray.direction.x;
            float tmin = std::min(tx1, tx2), tmax = std::max(tx1, tx2);
            float ty1 = (min.y - ray.origin.y) / ray.direction.y, ty2 = (max.y - ray.origin.y) / ray.direction.y;
            tmin = std::max(tmin, std::min(ty1, ty2)), tmax = std::min(tmax, std::max(ty1, ty2));
            float tz1 = (min.z - ray.origin.z) / ray.direction.z, tz2 = (max.z - ray.origin.z) / ray.direction.z;
            tmin = std::max(tmin, std::min(tz1, tz2)), tmax = std::min(tmax, std::max(tz1, tz2));
            return tmax >= tmin && tmin < raycast_distance && tmax > 0;
        }

        void Reset() {
            min = {};
            max = {};
        }

        void Expand(const glm::vec3& position) {
            min = glm::min(min, position);
            max = glm::max(max, position);
        }

        void Expand(const std::span<const glm::vec3>& positions) {
            for(const glm::vec3& position : positions)
            {
                min = glm::min(min, position);
                max = glm::max(max, position);
            }
        }

        void Encapsulate(const std::span<const glm::vec3>& positions) {
            min.x = min.y = min.z = std::numeric_limits<float>::max();
            max.x = max.y = max.z = std::numeric_limits<float>::min();
            Expand(positions);
        }

        glm::vec3 Extent()
        {
            return max - min;
        }
    };
}
