//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#ifndef RENDERER_SPHERE_RENDERER_H
#define RENDERER_SPHERE_RENDERER_H
#include "renderer.h"

namespace cg {
    class sphere_renderer : public renderer {
    private:
        float radius_ = 1.f;

    public:
        void SetRadius(float radius);

        [[nodiscard]] bool
        Intersects(const ray &ray, glm::vec3 &intersection, glm::vec3 &normal, float& raycast_distance, float max_distance) const override;
    };
}

#endif //RENDERER_SPHERE_RENDERER_H
