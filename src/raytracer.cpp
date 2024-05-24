//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#define ACCUMULATE_LIGHTING
// #define UNLIT_SHADING
// #define DEBUG_NORMALS
// #define DEBUG_UVS

#include "color.h"
#include "raytracer.h"
#include "materials/material.h"

namespace cg {
    bool raytracer::IntersectRenderers(const cg::ray &ray,
                                       const cg::scene &scene,
                                       float max_raycast_distance) {
        for (const std::shared_ptr <cg::renderer> &renderer: scene.GetRenderers()) {
            glm::vec3 local_intersection{};
            glm::vec3 local_normal{};
            glm::vec3 local_barycentric_coords{};
            glm::vec2 local_uv{};
            float local_raycast_distance{};
            if (renderer->Intersects(ray,
                                     local_intersection,
                                     local_normal,
                                     local_raycast_distance,
                                     local_barycentric_coords,
                                     local_uv,
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
                                     glm::vec2& uv,
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
            glm::vec3 local_barycentric_coords{};
            glm::vec2 local_uv{};
            float local_raycast_distance{};
            if (renderers[i]->Intersects(ray,
                                         local_intersection,
                                         local_normal,
                                         local_raycast_distance,
                                         local_barycentric_coords,
                                         local_uv,
                                         max_raycast_distance)) {
                if (local_raycast_distance < shortest_intersection) {
                    intersected_renderer_index = i;
                    shortest_intersection = local_raycast_distance;
                    intersection = local_intersection;
                    normal = local_normal;
                    uv = local_uv;
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

    void raytracer::Raycast(const cg::ray& ray, const cg::scene& scene, glm::vec4& accumulated_color,
                 glm::vec3& intersection,
                 glm::vec3& normal,
                 glm::vec2& uv,
                 int bounce_index,
                 int max_bounces,
                 float max_raycast_distance)
    {
        std::shared_ptr<cg::material> material;
        if (RaycastRenderers(ray, scene, intersection, normal, uv, max_raycast_distance, material)) {
            glm::vec4 albedo_color{};
            glm::vec3 emission_color{};
            float metallic = 0.f;
            float rougness = 0.f;

            if(material) {
                albedo_color = material->albedo_color;
                emission_color = material->emission_color;
                metallic = material->metallic;
                rougness = material->roughness;

                const std::shared_ptr<cg::texture>& albedo_texture = material->albedo_texture;
                if(albedo_texture) {
                    albedo_color = albedo_texture->SampleColor(uv);
                }
            }

            accumulated_color.x += emission_color.x;
            accumulated_color.y += emission_color.y;
            accumulated_color.z += emission_color.z;

#if defined(DEBUG_NORMALS)
            albedo_color.x = normal.x;
            albedo_color.y = normal.y;
            albedo_color.z = normal.z;
#elif defined(DEBUG_UVS)
            albedo_color.x = uv.x;
            albedo_color.y = uv.y;
            albedo_color.z = 0.f;
#endif

# if defined(ACCUMULATE_LIGHTING)
            for (const std::shared_ptr<cg::light> &light: scene.GetLights()) {
                glm::vec3 light_direction = glm::normalize(light->GetTransform().position - intersection);
                float light_distance = glm::distance(light->GetTransform().position, intersection);

                cg::ray light_ray{intersection, light_direction};
                if (!IntersectRenderers(light_ray, scene, light_distance)) {
                    accumulated_color += albedo_color * light->GetColor(light_ray, normal);
                }
            }
#elif defined(UNLIT_SHADING)
            accumulated_color += albedo_color;
#endif

            if(metallic > 0.f && bounce_index < max_bounces)
            {
                Raycast({intersection, normal}, scene, accumulated_color,
                        intersection, normal, uv, bounce_index + 1,
                        max_bounces, max_raycast_distance);
            }
        }
    }

    void raytracer::RaycastCamera(
            const cg::scene &scene,
            const cg::camera &camera_,
            std::vector <u_char> &output_data,
            int32_t width,
            int32_t height,
            int32_t channels,
            float max_raycast_distance,
            int samples_per_pixel) {

        // prepass renderers
        for (const std::shared_ptr <cg::renderer> &renderer: scene.GetRenderers()) {
            renderer->Prepass();
        }

        const int total_pixels = width * height;
        output_data.resize(total_pixels * channels);

        glm::mat4x4 p = camera_.GetProjectionMatrix();
        glm::mat4x4 v = camera_.GetViewMatrix();
        glm::mat4x4 vp = p * v;

        // from clip space to world space
        glm::mat4x4 vpi = glm::inverse(vp);

        glm::vec2 texel_size{1.f / (float)width, 1.f / (float)height};
        for (int i = 0; i < total_pixels; i++) {
            int x = i % width;
            int y = i / width;

            glm::vec4 accumulated_color{};

            for(int j = 0; j < samples_per_pixel; j++) {
                glm::vec2 sample_position {
                        -1.f + ((float) x / (float) (width - 1)) * 2.f,
                        1.f - ((float) y / (float) (height - 1)) * 2.f
                };

                // Apply random sampling
                if(samples_per_pixel > 1)
                    sample_position += RandomPositionInsideUnitDisc() * texel_size;

                glm::vec4 destNear = vpi * glm::vec4(sample_position.x, sample_position.y, -1.f, 1.f);
                destNear.x /= destNear.w;
                destNear.y /= destNear.w;
                destNear.z /= destNear.w;

                glm::vec4 destFar = vpi * glm::vec4(sample_position.x, sample_position.y, 1.f, 1.f);
                destFar.x /= destFar.w;
                destFar.y /= destFar.w;
                destFar.z /= destFar.w;

                glm::vec3 direction = glm::normalize(glm::vec3(destFar) - glm::vec3(destNear));
                cg::ray ray{destNear, direction};

                // raycast
                glm::vec3 intersection{};
                glm::vec3 normal{};
                glm::vec2 uv{};
                const int max_bounces = 3;

                Raycast(ray, scene, accumulated_color, intersection, normal, uv, 0, max_bounces, max_raycast_distance);
            }

            // Average by samples
            accumulated_color /= (float)samples_per_pixel;

            // gamma correction
            float const gamma = 1.f / 2.2f;
            accumulated_color = glm::pow(accumulated_color, {gamma, gamma, gamma, gamma});
            accumulated_color = glm::clamp(accumulated_color, 0.f, 1.f);

            const int j = (y * width + x) * channels;
            output_data[j + 0] = accumulated_color.x * 255;
            output_data[j + 1] = accumulated_color.y * 255;
            output_data[j + 2] = accumulated_color.z * 255;
        }
    }
}