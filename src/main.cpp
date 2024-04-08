//
// Created by Jaroslav Stehlik on 18.03.2024.
//


#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "json.hpp"

#include "scene.h"
#include "camera.h"
#include "renderers/sphere_renderer.h"
#include "renderers/plane_renderer.h"
#include "renderers/mesh_renderer.h"
#include "lights/point_light.h"
#include "loaders/gltf_loader.h"
#include "raytracer.h"
#include "resources.h"

int main() {
    // prepare resources
    cg::resources resources{};

    cg::gltf_loader gltf_loader{};

    std::shared_ptr<cg::mesh> box_mesh = std::make_shared<cg::mesh>();
    if(gltf_loader.LoadModel("box.glb", *box_mesh)) {
        resources.AddMesh("box.glb", box_mesh);
    }

    std::shared_ptr<cg::mesh> monkey_mesh = std::make_shared<cg::mesh>();
    if(gltf_loader.LoadModel("monkey.glb", *box_mesh)) {
        resources.AddMesh("monkey.glb", box_mesh);
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

    cg::mesh_renderer box_renderer {};
    box_renderer.SetTransform({glm::vec3(0.f, 0.f, 0.f),
                               glm::vec3(0, 45.f, 0),
                               glm::vec3(0.1f, 0.1f, 0.1f)});
    box_renderer.SetMesh(resources.GetMesh("box.glb"));

    cg::mesh_renderer monkey_renderer {};
    monkey_renderer.SetTransform({glm::vec3(0.f, 0.f, 0.f),
                               glm::vec3(0, -45.f, 0),
                               glm::vec3(0.1f, 0.1f, 0.1f)});
    monkey_renderer.SetMesh(resources.GetMesh("monkey.glb"));

    cg::scene scene_{};
    scene_.AddRenderer(std::make_shared<cg::plane_renderer>(plane));
    /*
    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere));
    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere2));
    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere3));
    scene_.AddRenderer(std::make_shared<cg::mesh_renderer>(box_renderer));
     */

    scene_.AddRenderer(std::make_shared<cg::mesh_renderer>(monkey_renderer));

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

    cg::raytracer raytracer{};

    auto start = std::chrono::high_resolution_clock::now();
    raytracer.RaycastCamera(scene_, camera_, output_data, width, height);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "rendering took: " << duration.count() << "s" << std::endl;

    // save image to drive
    const int32_t channel_count = 3;
    stbi_write_png("image.png", width, height, channel_count, output_data.data(), width * channel_count);
    return 0;
}