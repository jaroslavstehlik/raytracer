//
// Created by Jaroslav Stehlik on 22.04.2024.
//
#pragma once
#include "glm/glm.hpp"
#include "ray.h"

namespace cg
{
    inline glm::vec2 BarycentricInterpolate(glm::vec2 a, glm::vec2 b,glm::vec2 c, glm::vec2 barycentric_coords)
    {
        return barycentric_coords.x * a + barycentric_coords.y * b + (1.f - barycentric_coords.x - barycentric_coords.y) * c;
    }

    inline glm::vec2 BarycentricInterpolate(glm::vec2 a, glm::vec2 b,glm::vec2 c, glm::vec3 barycentric_coords)
    {
        return barycentric_coords.x * a + barycentric_coords.y * b + barycentric_coords.z * c;
    }

    inline glm::vec3 BarycentricInterpolate(glm::vec3 a, glm::vec3 b,glm::vec3 c, glm::vec2 barycentric_coords)
    {
        return barycentric_coords.x * a + barycentric_coords.y * b + (1.f - barycentric_coords.x - barycentric_coords.y) * c;
    }

    inline glm::vec3 BarycentricInterpolate(glm::vec3 a, glm::vec3 b,glm::vec3 c, glm::vec3 barycentric_coords)
    {
        return barycentric_coords.x * a + barycentric_coords.y * b + barycentric_coords.z * c;
    }

    inline glm::vec3 Reflect(const glm::vec3& direction, const glm::vec3& normal)
    {
        // 𝑟 = 𝑑−2(𝑑⋅𝑛)𝑛
        return direction - 2.f * glm::dot(direction, normal) * normal;
    }

    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    inline bool IntersectTriangle(const cg::ray& ray,
                                const glm::vec3& vertex0,
                                const glm::vec3& vertex1,
                                const glm::vec3& vertex2,
                                float& raycast_distance,
                                glm::vec3& barycentric_coords) {

        constexpr float epsilon = std::numeric_limits<float>::epsilon();
        const glm::vec3 edge1 = vertex1 - vertex0;
        const glm::vec3 edge2 = vertex2 - vertex0;
        const glm::vec3 ray_cross_e2 = cross(ray.direction, edge2);
        const float det = dot(edge1, ray_cross_e2);
        if (det > -epsilon && det < epsilon)
            return false; // ray parallel to triangle

        const float inv_det = 1 / det;
        const glm::vec3 s = ray.origin - vertex0;
        barycentric_coords.x = inv_det * dot(s, ray_cross_e2);
        if (barycentric_coords.x < 0 || barycentric_coords.x > 1)
            return false;

        const glm::vec3 s_cross_e1 = cross(s, edge1);
        barycentric_coords.y = inv_det * dot(ray.direction, s_cross_e1);
        if (barycentric_coords.y < 0 || barycentric_coords.x + barycentric_coords.y > 1)
            return false;

        const float t = inv_det * dot(edge2, s_cross_e1);

        if (t <= epsilon)
            return false;

        barycentric_coords.z = 1.f - barycentric_coords.x - barycentric_coords.y;
        raycast_distance = t;
        return true;
    }
}