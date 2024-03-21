//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "scene.h"
#include "camera.h"
#include "renderers/sphere_renderer.h"

void CoutVector3(const char* name, const glm::vec3& pos)
{
    std::cout << name << "x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
}

void RaycastCamera(const scene& scene_, const camera& camera_,
                   int width, int height, float raycast_distance = 10.f) {

    const int total_pixels = width * height;
    std::string line = "";

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

        if(x == 0) {
            std::cout << line << std::endl;
            line = "";
        }

        line += std::to_string(intersects);
    }
}

int main() {
    sphere_renderer sphere {};
    sphere.SetRadius(0.5f);
    sphere.SetTransform({glm::vec3(0.0f, 0.0f, 0.0f),
                         glm::vec3(0, 0, 0),
                         glm::vec3(1, 1, 1)});

    scene scene_{};
    scene_.AddRenderer(std::make_shared<sphere_renderer>(sphere));

    camera camera_{};
    camera_.SetPosition(glm::vec3(0.f, 0.f, -1.f));
    camera_.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    camera_.SetAspectRatio(1.f);
    camera_.SetNearClipPlane(0.1f);
    camera_.SetFarClipPlane(10.f);
    camera_.SetVerticalFov(60.f);

    RaycastCamera(scene_, camera_, 32, 32);
    return 0;
}