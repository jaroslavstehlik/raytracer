//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#ifndef RENDERER_SPHERE_RENDERER_H
#define RENDERER_SPHERE_RENDERER_H
#include "renderer.h"

class sphere_renderer : public renderer {
private:
    float radius_ = 1.f;

public:
    void SetRadius(float radius);
    [[nodiscard]] bool Intersects(const ray &ray, float raycast_distance) const override;
};

#endif //RENDERER_SPHERE_RENDERER_H
