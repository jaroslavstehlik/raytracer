//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "scene.h"
#include "camera.h"
#include "renderers/sphere_renderer.h"
#include "stb_image_write.h"

void CoutVector3(const char* name, const glm::vec3& pos)
{
    std::cout << name << "x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
}

void RaycastCamera(const scene& scene_, const camera& camera_, std::vector<u_char>& output_data,
                   int width, int height, float raycast_distance = 10.f) {

    const int total_pixels = width * height;

    glm::mat4x4 p = camera_.GetProjectionMatrix();
    glm::mat4x4 v = camera_.GetViewMatrix();
    glm::mat4x4 pi = glm::inverse(p);
    glm::mat4x4 vpi = v * pi;

    for(int i = 0; i < total_pixels; i++) {
        int x = i % width;
        int y = i / width;

        float xf = -1.f + ((float)x / (float)(width - 1)) * 2.f;
        float yf = -1.f + ((float)y / (float)(height - 1)) * 2.f;

        glm::vec4 destNear = vpi * glm::vec4 (xf, yf, -1.f, 1.f);
        destNear.x /= destNear.w;
        destNear.y /= destNear.w;
        destNear.z /= destNear.w;

        glm::vec4 destFar = vpi * glm::vec4 (xf, yf, 1.f, 1.f);
        destFar.x /= destFar.w;
        destFar.y /= destFar.w;
        destFar.z /= destFar.w;

        glm::vec3 direction = glm::normalize(glm::vec3(destFar) - glm::vec3(destNear));
        ray ray_ {destNear, direction};

        bool intersects = false;
        for(const std::shared_ptr<renderer>& renderer : scene_.GetRenderers()) {
            intersects |= renderer->Intersects(ray_, raycast_distance);
        }

        output_data.emplace_back(intersects * 255);
    }
}

int main() {
    // setup scene
    sphere_renderer sphere {};
    sphere.SetRadius(0.1f);
    sphere.SetTransform({glm::vec3(-0.2f, 0.0f, 0.0f),
                         glm::vec3(0, 0, 0),
                         glm::vec3(1, 1, 1)});

    sphere_renderer sphere2 {};
    sphere2.SetRadius(0.1f);
    sphere2.SetTransform({glm::vec3(0.2f, 0.0f, 0.0f),
                         glm::vec3(0, 0, 0),
                         glm::vec3(1, 1, 1)});

    scene scene_{};
    scene_.AddRenderer(std::make_shared<sphere_renderer>(sphere));
    scene_.AddRenderer(std::make_shared<sphere_renderer>(sphere2));

    const int32_t width = 512;
    const int32_t height = 512;

    camera camera_{};
    camera_.SetPosition(glm::vec3(0.f, 0.f, -1.f));
    camera_.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    camera_.SetAspectRatio((float)width / (float)height);
    camera_.SetNearClipPlane(0.1f);
    camera_.SetFarClipPlane(10.f);
    camera_.SetVerticalFov(60.f);

    // raycast camera
    std::vector<u_char> output_data{};
    RaycastCamera(scene_, camera_, output_data, width, height);

    // save image to drive
    const int32_t channel_count = 1;
    stbi_write_png("image.png", width, height, channel_count, output_data.data(), width * channel_count);
    return 0;
}