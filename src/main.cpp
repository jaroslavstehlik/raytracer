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
#include "materials/material.h"
#include "lights/point_light.h"
#include "loaders/gltf_loader.h"
#include "raytracer.h"
#include "resources.h"

int main() {
    // prepare resources

    cg::resources resources{};

    cg::material white_material{nullptr, glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), 0.f, 1.f};
    cg::material blue_material{nullptr, glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f),  0.f, 1.f};
    cg::material red_material{nullptr, glm::vec4(1.f, 0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f),  0.f, 1.f};
    cg::material metal_material{nullptr, glm::vec4(0.1f, 0.1f, 0.1f, 1.f), glm::vec3(0.f, 0.f, 0.f), 1.f, 0.f};
    cg::material emissive_material{nullptr, glm::vec4(0.0f, 0.0f, 0.0f, 1.f), glm::vec3(1.f, 0.f, 0.f), 0.f, 0.f};
    resources.AddMaterial("white", std::make_shared<cg::material>(white_material));
    resources.AddMaterial("blue", std::make_shared<cg::material>(blue_material));
    resources.AddMaterial("red", std::make_shared<cg::material>(red_material));
    resources.AddMaterial("metal", std::make_shared<cg::material>(metal_material));
    resources.AddMaterial("emissive", std::make_shared<cg::material>(emissive_material));

    // setup scene
    cg::plane_renderer plane {};
    plane.SetTransform({glm::vec3(0.0f, -0.05f, 0.0f),
                       glm::vec3(0, 0, 0.f),
                       glm::vec3(1, 1, 1)});
    plane.SetMaterial(resources.GetMaterial("white"));

    cg::sphere_renderer sphere {};
    sphere.SetRadius(0.1f);
    sphere.SetTransform({glm::vec3(-0.3f, 0.05f, 0.0f),
                         glm::vec3(0, 0, 0),
                         glm::vec3(1, 1, 1)});
    sphere.SetMaterial(resources.GetMaterial("metal"));

    cg::sphere_renderer sphere2 {};
    sphere2.SetRadius(0.1f);
    sphere2.SetTransform({glm::vec3(0.3f, 0.05f, 0.0f),
                         glm::vec3(0, 0, 0),
                         glm::vec3(1, 1, 1)});
    sphere2.SetMaterial(resources.GetMaterial("metal"));

    cg::sphere_renderer sphere3 {};
    sphere3.SetRadius(0.2f);
    sphere3.SetTransform({glm::vec3(0.0f, 0.1f, 1.0f),
                          glm::vec3(0, 0, 0),
                          glm::vec3(1, 1, 1)});
    sphere3.SetMaterial(resources.GetMaterial("emissive"));

    cg::point_light point_light {};
    point_light.SetColor(glm::vec4(1.f, 0.0f, 0.f, 1.f));
    //point_light.SetColor(glm::vec4(1.f, 1.0f, 1.f, 1.f));
    point_light.SetTransform({glm::vec3(0.5f, 0.3f, -1.f),
                              glm::vec3(0, 0, 0),
                              glm::vec3(1, 1, 1)});

    cg::point_light point_light2 {};
    point_light2.SetColor(glm::vec4(0.f, 0.0f, 1.f, 1.f));
    //point_light2.SetColor(glm::vec4(1.f, 1.0f, 1.f, 1.f));
    point_light2.SetTransform({glm::vec3(-0.5f, 0.3f, -1.f),
                              glm::vec3(0, 0, 0),
                              glm::vec3(1, 1, 1)});



    cg::scene scene_{};

    scene_.AddRenderer(std::make_shared<cg::plane_renderer>(plane));

    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere));
    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere2));
    scene_.AddRenderer(std::make_shared<cg::sphere_renderer>(sphere3));

    scene_.AddLight(std::make_shared<cg::point_light>(point_light));
    scene_.AddLight(std::make_shared<cg::point_light>(point_light2));

    cg::gltf_loader gltf_loader{};

    // box
    cg::gltf_loader_result box_loader_result = gltf_loader.LoadModel("box.glb", resources);
    if(box_loader_result.success) {
        cg::mesh_renderer box_renderer{};
        box_renderer.SetTransform({glm::vec3(0.0f, 0.1f, 0.0f),
                                   glm::vec3(0, 45.f, 0),
                                   glm::vec3(0.1f, 0.1f, 0.1f)});

        if(box_loader_result.mesh_names.size() > 0) {
            box_renderer.SetMesh(resources.GetMesh(box_loader_result.mesh_names[0]));
        }

        if (box_loader_result.material_names.size() > 0) {
            box_renderer.SetMaterial(resources.GetMaterial(box_loader_result.material_names[0]));
        } else {
            box_renderer.SetMaterial(resources.GetMaterial("white"));
        }

        scene_.AddRenderer(std::make_shared<cg::mesh_renderer>(box_renderer));
    }

    // icosphere
    cg::gltf_loader_result ico_loader_result = gltf_loader.LoadModel("ico.glb", resources);
    if(ico_loader_result.success) {
        cg::mesh_renderer ico_renderer{};
        ico_renderer.SetTransform({glm::vec3(0.0f, 0.1f, 0.0f),
                                   glm::vec3(0, 135.f, 0),
                                   glm::vec3(0.1f, 0.1f, 0.1f)});

        if(ico_loader_result.mesh_names.size() > 0) {
            ico_renderer.SetMesh(resources.GetMesh(ico_loader_result.mesh_names[0]));
        }

        if (ico_loader_result.material_names.size() > 0) {
            ico_renderer.SetMaterial(resources.GetMaterial(ico_loader_result.material_names[0]));
        } else {
            ico_renderer.SetMaterial(resources.GetMaterial("white"));
        }

        scene_.AddRenderer(std::make_shared<cg::mesh_renderer>(ico_renderer));
    }

    // monkey
    cg::gltf_loader_result monkey_loader_result = gltf_loader.LoadModel("monkey.glb", resources);
    if(monkey_loader_result.success) {
        cg::mesh_renderer monkey_renderer{};
        monkey_renderer.SetTransform({glm::vec3(0.0f, 0.05f, -0.1f),
                                      glm::vec3(0, 180.f, 0),
                                      glm::vec3(0.1f, 0.1f, 0.1f)});

        if(monkey_loader_result.mesh_names.size() > 0) {
            monkey_renderer.SetMesh(resources.GetMesh(monkey_loader_result.mesh_names[0]));
        }

        if (monkey_loader_result.material_names.size() > 0) {
            monkey_renderer.SetMaterial(resources.GetMaterial(monkey_loader_result.material_names[0]));
        } else {
            monkey_renderer.SetMaterial(resources.GetMaterial("white"));
        }

        scene_.AddRenderer(std::make_shared<cg::mesh_renderer>(monkey_renderer));
    }

    const int32_t width = 512;
    const int32_t height = 512;

    cg::camera camera_{};
    camera_.SetPosition(glm::vec3(-0.f, 0.5f, -1.0f));
    camera_.SetRotation(glm::vec3(25.0f, 0.0f, 0.0f));
    camera_.SetAspectRatio((float)width / (float)height);
    camera_.SetNearClipPlane(0.1f);
    camera_.SetFarClipPlane(10.f);
    camera_.SetVerticalFov(60.f);

    // raycast camera
    std::vector<u_char> output_data{};

    const int32_t channel_count = 3;
    cg::raytracer raytracer{};

    auto start = std::chrono::high_resolution_clock::now();
    raytracer.RaycastCamera(scene_, camera_, output_data, width, height, channel_count);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "rendering took: " << duration.count() << "s" << std::endl;

    // save image to drive
    stbi_write_png("image.png", width, height, channel_count, output_data.data(), width * channel_count);
    return 0;
}