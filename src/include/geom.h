//
// Created by Jaroslav Stehlik on 22.04.2024.
//
#pragma once
#include "glm/glm.hpp"
#include "ray.h"

namespace cg
{
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    bool IntersectTriangle(const cg::ray& ray,
                                const glm::vec3& vertex0,
                                const glm::vec3& vertex1,
                                const glm::vec3& vertex2,
                                float& raycast_distance,
                                glm::vec2& uv) {

        constexpr float epsilon = std::numeric_limits<float>::epsilon();
        const glm::vec3 edge1 = vertex1 - vertex0;
        const glm::vec3 edge2 = vertex2 - vertex0;
        const glm::vec3 ray_cross_e2 = cross(ray.direction, edge2);
        const float det = dot(edge1, ray_cross_e2);
        if (det > -epsilon && det < epsilon)
            return false; // ray parallel to triangle

        const float inv_det = 1 / det;
        const glm::vec3 s = ray.origin - vertex0;
        uv.x = inv_det * dot(s, ray_cross_e2);
        if (uv.x < 0 || uv.x > 1)
            return false;

        const glm::vec3 s_cross_e1 = cross(s, edge1);
        uv.y = inv_det * dot(ray.direction, s_cross_e1);
        if (uv.y < 0 || uv.x + uv.y > 1)
            return false;

        const float t = inv_det * dot(edge2, s_cross_e1);

        if (t <= epsilon)
            return false;

        raycast_distance = t;
        return true;
    }
}