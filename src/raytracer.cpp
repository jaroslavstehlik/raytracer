//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "raytracer.h"
#include "materials/material.h"

namespace cg {
    bool raytracer::IntersectRenderers(const cg::ray &ray,
                                       const cg::scene &scene,
                                       float max_raycast_distance) {
        for (const std::shared_ptr <cg::renderer> &renderer: scene.GetRenderers()) {
            glm::vec3 local_intersection{};
            glm::vec3 local_normal{};
            float local_raycast_distance{};
            if (renderer->Intersects(ray, local_intersection, local_normal, local_raycast_distance,
                                     max_raycast_distance)) {
                return true;
            }
        }
        return false;
    }

    bool raytracer::RaycastRenderers(const cg::ray &ray,
                                     const cg::scene &scene,
                                     glm::vec3 &intersection,
                                     glm::vec3 &normal,
                                     float max_raycast_distance,
                                     std::shared_ptr<cg::material>& material) {
        float shortest_intersection = std::numeric_limits<float>::max();
        int intersected_renderer_index = -1;

        // raycast renderers
        std::span<const std::shared_ptr<cg::renderer>> renderers = scene.GetRenderers();
        for(int i = 0; i < renderers.size(); i++)
        {
            glm::vec3 local_intersection{};
            glm::vec3 local_normal{};
            float local_raycast_distance{};
            if (renderers[i]->Intersects(ray, local_intersection, local_normal, local_raycast_distance, max_raycast_distance)) {
                if (local_raycast_distance < shortest_intersection) {
                    intersected_renderer_index = i;
                    shortest_intersection = local_raycast_distance;
                    intersection = local_intersection;
                    normal = local_normal;
                }
            }
        }

        if(intersected_renderer_index != -1) {
            material = renderers[intersected_renderer_index]->GetMaterial();
        } else {
            material = nullptr;
        }
        return shortest_intersection != std::numeric_limits<float>::max();
    }

    void raytracer::Raycast(const cg::ray& ray, const cg::scene& scene, glm::vec3& accumulated_color,
                 glm::vec3& intersection, glm::vec3& normal, int bounce_index,
                 int max_bounces, float max_raycast_distance)
    {
        std::shared_ptr<cg::material> material;
        if (RaycastRenderers(ray, scene, intersection, normal, max_raycast_distance, material)) {
            glm::vec3 albedo_color{};
            float rougness = 0.f;
            float metallic = 0.f;

            if(material) {
                albedo_color = material->albedo;
                rougness = material->roughness;
                metallic = material->metallic;
            }

            for (const std::shared_ptr<cg::light> &light: scene.GetLights()) {
                glm::vec3 light_direction = glm::normalize(light->GetTransform().position - intersection);
                float light_distance = glm::distance(light->GetTransform().position, intersection);

                cg::ray light_ray{intersection, light_direction};
                if (!IntersectRenderers(light_ray, scene, light_distance)) {
                    accumulated_color += albedo_color * light->GetColor(light_ray, normal);
                }
            }

            if(metallic > 0.f && bounce_index < max_bounces)
            {
                Raycast({intersection, normal}, scene, accumulated_color,
                        intersection, normal, bounce_index + 1,
                        max_bounces, max_raycast_distance);
            }
        }
    }

    void raytracer::RaycastCamera(const cg::scene &scene, const cg::camera &camera_, std::vector <u_char> &output_data,
                       int width, int height, float max_raycast_distance) {

        // prepass renderers
        for (const std::shared_ptr <cg::renderer> &renderer: scene.GetRenderers()) {
            renderer->RecalculateBounds();
        }

        const int total_pixels = width * height;

        glm::mat4x4 p = camera_.GetProjectionMatrix();
        glm::mat4x4 v = camera_.GetViewMatrix();
        glm::mat4x4 pi = glm::inverse(p);
        glm::mat4x4 vpi = v * pi;

        for (int i = 0; i < total_pixels; i++) {
            int x = i % width;
            int y = i / width;

            // flip x and y
            float xf = 1.f - ((float) x / (float) (width - 1)) * 2.f;
            float yf = 1.f - ((float) y / (float) (height - 1)) * 2.f;

            glm::vec4 destNear = vpi * glm::vec4(xf, yf, -1.f, 1.f);
            destNear.x /= destNear.w;
            destNear.y /= destNear.w;
            destNear.z /= destNear.w;

            glm::vec4 destFar = vpi * glm::vec4(xf, yf, 1.f, 1.f);
            destFar.x /= destFar.w;
            destFar.y /= destFar.w;
            destFar.z /= destFar.w;

            glm::vec3 direction = glm::normalize(glm::vec3(destFar) - glm::vec3(destNear));
            cg::ray ray{destNear, direction};

            // raycast
            glm::vec3 accumulated_color{};
            glm::vec3 intersection{};
            glm::vec3 normal{};
            const int max_bounces = 3;

            Raycast(ray, scene, accumulated_color, intersection, normal, 0, max_bounces, max_raycast_distance);

            accumulated_color = glm::clamp(accumulated_color, 0.f, 1.f);
            output_data.emplace_back(accumulated_color.x * 255);
            output_data.emplace_back(accumulated_color.y * 255);
            output_data.emplace_back(accumulated_color.z * 255);
        }
    }
}