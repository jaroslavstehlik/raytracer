//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "raytracer.h"

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
                                     float &raycast_distance,
                                     float max_raycast_distance) {
        float shortest_intersection = std::numeric_limits<float>::max();

        // prepass renderers
        for (const std::shared_ptr <cg::renderer> &renderer: scene.GetRenderers()) {
            renderer->RecalculateBounds();
        }

        // raycast renderers
        for (const std::shared_ptr <cg::renderer> &renderer: scene.GetRenderers()) {
            glm::vec3 local_intersection{};
            glm::vec3 local_normal{};
            float local_raycast_distance{};
            if (renderer->Intersects(ray, local_intersection, local_normal, local_raycast_distance,
                                     max_raycast_distance)) {
                if (local_raycast_distance < shortest_intersection) {
                    shortest_intersection = local_raycast_distance;
                    raycast_distance = local_raycast_distance;
                    intersection = local_intersection;
                    normal = local_normal;
                }
            }
        }

        return shortest_intersection != std::numeric_limits<float>::max();
    }

    void raytracer::RaycastCamera(const cg::scene &scene_, const cg::camera &camera_, std::vector <u_char> &output_data,
                       int width, int height, float max_raycast_distance) {

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
            cg::ray ray_{destNear, direction};

            // raycast
            glm::vec3 intersection{};
            glm::vec3 normal{};
            float raycast_distance{};

            if (RaycastRenderers(ray_, scene_, intersection, normal, raycast_distance, max_raycast_distance)) {
                glm::vec3 color{};

                for (const std::shared_ptr <cg::light> &light: scene_.GetLights()) {
                    glm::vec3 light_direction = glm::normalize(light->GetTransform().position - intersection);
                    float light_distance = glm::distance(light->GetTransform().position, intersection);

                    cg::ray light_ray{intersection, light_direction};
                    if (!IntersectRenderers(light_ray, scene_, light_distance)) {
                        color += light->GetColor(light_ray, normal);
                    }
                }

                // normalize color
                color.x = glm::clamp(color.x, 0.f, 1.f);
                color.y = glm::clamp(color.y, 0.f, 1.f);
                color.z = glm::clamp(color.z, 0.f, 1.f);

                output_data.emplace_back(color.x * 255);
                output_data.emplace_back(color.y * 255);
                output_data.emplace_back(color.z * 255);
            } else {
                output_data.emplace_back(0);
                output_data.emplace_back(0);
                output_data.emplace_back(0);
            }
        }
    }
}