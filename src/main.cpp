//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#define _CRT_SECURE_NO_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include <iostream>
#include "scene.h"
#include "camera.h"
#include "renderers/sphere_renderer.h"
#include "renderers/plane_renderer.h"
#include "lights/point_light.h"

// begin tiny GLTF
#include "stb_image.h"
#include "stb_image_write.h"
#include "json.hpp"

#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#include "tiny_gltf.h"
// end tinyGLTF

void CoutVector3(const char* name, const glm::vec3& pos)
{
    std::cout << name << "x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
}

glm::vec3 PointLight(const glm::vec3& light_position, const glm::vec3& intersection, const glm::vec3& normal, const glm::vec3& color)
{
    glm::vec3 light_direction = glm::normalize(light_position - intersection);
    float light_distance = glm::distance(intersection, light_position);
    float light = glm::max(0.f, glm::dot(normal, light_direction));
    float attenuation = 1.0f / glm::max(0.0001f, light_distance);
    float light_normalized = glm::clamp(light * attenuation, 0.f, 1.f);
    return color * light_normalized;
}

bool IntersectRenderers(const cg::ray& ray,
                      const cg::scene& scene,
                      float max_raycast_distance)
{
    for(const std::shared_ptr<cg::renderer>& renderer : scene.GetRenderers()) {
        glm::vec3 local_intersection{};
        glm::vec3 local_normal{};
        float local_raycast_distance{};
        if(renderer->Intersects(ray, local_intersection, local_normal, local_raycast_distance, max_raycast_distance))
        {
            return true;
        }
    }
    return false;
}

bool RaycastRenderers(const cg::ray& ray,
                      const cg::scene& scene,
                      glm::vec3& intersection,
                      glm::vec3& normal,
                      float& raycast_distance,
                      float max_raycast_distance)
{
    float shortest_intersection = std::numeric_limits<float>::max();

    for(const std::shared_ptr<cg::renderer>& renderer : scene.GetRenderers()) {
        glm::vec3 local_intersection{};
        glm::vec3 local_normal{};
        float local_raycast_distance{};
        if(renderer->Intersects(ray, local_intersection, local_normal, local_raycast_distance, max_raycast_distance)) {
            if(local_raycast_distance < shortest_intersection) {
                shortest_intersection = local_raycast_distance;
                raycast_distance = local_raycast_distance;
                intersection = local_intersection;
                normal = local_normal;
            }
        }
    }

    return shortest_intersection != std::numeric_limits<float>::max();
}

void RaycastCamera(const cg::scene& scene_, const cg::camera& camera_, std::vector<u_char>& output_data,
                   int width, int height, float max_raycast_distance = 10.f) {

    const int total_pixels = width * height;

    glm::mat4x4 p = camera_.GetProjectionMatrix();
    glm::mat4x4 v = camera_.GetViewMatrix();
    glm::mat4x4 pi = glm::inverse(p);
    glm::mat4x4 vpi = v * pi;

    for(int i = 0; i < total_pixels; i++) {
        int x = i % width;
        int y = i / width;

        // flip x and y
        float xf = 1.f - ((float)x / (float)(width - 1)) * 2.f;
        float yf = 1.f - ((float)y / (float)(height - 1)) * 2.f;

        glm::vec4 destNear = vpi * glm::vec4 (xf, yf, -1.f, 1.f);
        destNear.x /= destNear.w;
        destNear.y /= destNear.w;
        destNear.z /= destNear.w;

        glm::vec4 destFar = vpi * glm::vec4 (xf, yf, 1.f, 1.f);
        destFar.x /= destFar.w;
        destFar.y /= destFar.w;
        destFar.z /= destFar.w;

        glm::vec3 direction = glm::normalize(glm::vec3(destFar) - glm::vec3(destNear));
        cg::ray ray_ {destNear, direction};

        // raycast
        glm::vec3 intersection{};
        glm::vec3 normal{};
        float raycast_distance{};

        if(RaycastRenderers(ray_, scene_, intersection, normal, raycast_distance, max_raycast_distance)) {
            glm::vec3 color{};

            for(const std::shared_ptr<cg::light>& light : scene_.GetLights()) {
                glm::vec3 light_direction = glm::normalize(light->GetTransform().position - intersection);
                float light_distance = glm::distance(light->GetTransform().position, intersection);

                cg::ray light_ray {intersection, light_direction};
                if(!IntersectRenderers(light_ray, scene_, light_distance)) {
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

int main() {
    std::string input_filename = "box.glb";
    std::string err;
    std::string warn;

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, input_filename);
    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }
    if (!err.empty()) {
        printf("ERR: %s\n", err.c_str());
    }
    if (!ret) {
        printf("Failed to load .glTF : %s\n", input_filename.c_str());
        exit(-1);
    }

    // setup scene
    cg::plane_renderer plane {};
    plane.SetTransform({glm::vec3(0.0f, -0.05f, 0.0f),
                       glm::vec3(0, 0, 0.f),
                       glm::vec3(1, 1, 1)});

    cg::sphere_renderer sphere {};
    sphere.SetRadius(0.1f);
    sphere.SetTransform({glm::vec3(-0.2f, 0.0f, 0.0f),
                         glm::vec3(0, 0, 0),
                         glm::vec3(1, 1, 1)});

    cg::sphere_renderer sphere2 {};
    sphere2.SetRadius(0.1f);
    sphere2.SetTransform({glm::vec3(0.2f, 0.0f, 0.0f),
                         glm::vec3(0, 0, 0),
                         glm::vec3(1, 1, 1)});

    cg::sphere_renderer sphere3 {};
    sphere3.SetRadius(0.2f);
    sphere3.SetTransform({glm::vec3(0.0f, 0.1f, -0.0f),
                          glm::vec3(0, 0, 0),
                          glm::vec3(1, 1, 1)});

    cg::point_light point_light {};
    point_light.SetColor(glm::vec3(1.f, 0.f, 0.f));
    point_light.SetTransform({glm::vec3(0.5f, 0.3f, -0.5f),
                              glm::vec3(0, 0, 0),
                              glm::vec3(1, 1, 1)});

    cg::point_light point_light2 {};
    point_light2.SetColor(glm::vec3(0.f, 0.f, 1.f));
    point_light2.SetTransform({glm::vec3(-0.5f, 0.3f, -0.5f),
                              glm::vec3(0, 0, 0),
                              glm::vec3(1, 1, 1)});

    cg::scene scene_{};
    scene_.AddRenderer(std::make_shared<cg::plane_renderer>(plane));
    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere));
    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere2));
    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere3));
    scene_.AddLight(std::make_shared<cg::point_light>(point_light));
    scene_.AddLight(std::make_shared<cg::point_light>(point_light2));

    const int32_t width = 512;
    const int32_t height = 512;

    cg::camera camera_{};
    camera_.SetPosition(glm::vec3(0.f, 0.5f, -0.7f));
    camera_.SetRotation(glm::vec3(25.0f, 0.0f, 0.0f));
    camera_.SetAspectRatio((float)width / (float)height);
    camera_.SetNearClipPlane(0.1f);
    camera_.SetFarClipPlane(10.f);
    camera_.SetVerticalFov(60.f);

    // raycast camera
    std::vector<u_char> output_data{};
    RaycastCamera(scene_, camera_, output_data, width, height);

    // save image to drive
    const int32_t channel_count = 3;
    stbi_write_png("image.png", width, height, channel_count, output_data.data(), width * channel_count);
    return 0;
}