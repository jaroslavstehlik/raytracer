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
                   int width, int height, float rycast_distance = 1.f) {

    const int total_pixels = width * height;
    std::string line = "";
    for(int i = 0; i < total_pixels; i++)
    {
        int x = i % width;
        int y = i / width;

        float xf = -0.5f + (float)x / (float)(width - 1);
        float yf = -0.5f + (float)y / (float)(height - 1);

        ray ray_ {camera_.GetPosition() + glm::vec3 (xf, yf, 0.f),
                  glm::vec3 (0.f, 0.f, 1.f)};

        bool intersects = false;
        for(const std::shared_ptr<renderer>& renderer : scene_.GetRenderers())
        {
            intersects |= renderer->Intersects(ray_, rycast_distance);
        }

        if(x == 0) {
            std::cout << line << std::endl;
            line = "";
        }

        line += std::to_string(intersects);
    }
}

int main()
{
    sphere_renderer sphere {};
    sphere.SetRadius(1.0f);
    sphere.SetTransform({glm::vec3(0.0f, 0.0f, 0),
                         glm::vec3(90, 0, 0),
                         glm::vec3(1, 1, 1)});

    scene scene_{};
    scene_.AddRenderer(std::make_shared<sphere_renderer>(sphere));

    camera camera_{};
    camera_.SetPosition(glm::vec3(0.0f, 0.0f, -0.5f));
    camera_.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    camera_.SetAspectRatio(1.f);
    camera_.SetNearClipPlane(0.1f);
    camera_.SetFarClipPlane(10.f);
    camera_.SetVerticalFov(60.f);

    RaycastCamera(scene_, camera_, 32, 32);
    return 0;
}