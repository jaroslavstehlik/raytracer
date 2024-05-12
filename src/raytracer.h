//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#pragma once
#include <locale>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene.h"
#include "camera.h"
namespace cg {
    class raytracer {
    private:
        bool IntersectRenderers(const cg::ray& ray,
                                const cg::scene& scene,
                                float max_raycast_distance);

        bool RaycastRenderers(const cg::ray& ray,
                              const cg::scene& scene,
                              glm::vec3& intersection,
                              glm::vec3& normal,
                              glm::vec2& uv,
                              float max_raycast_distance,
                              std::shared_ptr<cg::material>& material);

        void Raycast(const cg::ray& ray, const cg::scene& scene, glm::vec4& accumulated_color,
                     glm::vec3& intersection, glm::vec3& normal, glm::vec2& uv, int bounce_index,
                     int max_bounces, float max_raycast_distance);

    public:
        void RaycastCamera(const cg::scene& scene_, const cg::camera& camera_, std::vector<u_char>& output_data,
                           int width, int height, float max_raycast_distance = 10.f);
    };
}
