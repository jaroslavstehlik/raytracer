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

        bool Intersects(const cg::ray& ray) const {
            // https://tavianator.com/2011/ray_box.html

            float tmin = std::numeric_limits<float>::min();
            float tmax = std::numeric_limits<float>::max();

            if (ray.direction.x != 0.f) {
                float t1 = (min.x - ray.origin.x) / ray.direction.x;
                float t2 = (max.x - ray.origin.x) / ray.direction.x;
                tmin = std::max(tmin, std::min(t1, t2));
                tmax = std::min(tmax, std::max(t1, t2));
            }

            if (ray.direction.y != 0.f) {
                float t1 = (min.y - ray.origin.y) / ray.direction.y;
                float t2 = (max.y - ray.origin.y) / ray.direction.y;
                tmin = std::max(tmin, std::min(t1, t2));
                tmax = std::min(tmax, std::max(t1, t2));
            }

            if (ray.direction.z != 0.f) {
                float t1 = (min.z - ray.origin.z) / ray.direction.z;
                float t2 = (max.z - ray.origin.z) / ray.direction.z;
                tmin = std::max(tmin, std::min(t1, t2));
                tmax = std::min(tmax, std::max(t1, t2));
            }

            return tmin < tmax;
        }

        void Reset() {
            min = {};
            max = {};
        }

        void Expand(const glm::vec3& position) {
            min.x = std::min(position.x, min.x);
            min.y = std::min(position.y, min.y);
            min.z = std::min(position.z, min.z);

            max.x = std::max(position.x, max.x);
            max.y = std::max(position.y, max.y);
            max.z = std::max(position.z, max.z);
        }

        void Expand(const std::span<const glm::vec3>& positions) {
            for(const glm::vec3& position : positions)
            {
                min.x = std::min(position.x, min.x);
                min.y = std::min(position.y, min.y);
                min.z = std::min(position.z, min.z);

                max.x = std::max(position.x, max.x);
                max.y = std::max(position.y, max.y);
                max.z = std::max(position.z, max.z);
            }
        }

        void Encapsulate(const std::span<const glm::vec3>& positions) {
            min.x = min.y = min.z = std::numeric_limits<float>::max();
            max.x = max.y = max.z = std::numeric_limits<float>::min();
            Expand(positions);
        }
    };
}
