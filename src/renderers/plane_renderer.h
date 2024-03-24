//
// Created by Jaroslav Stehlik on 24.03.2024.
//

#ifndef RENDERER_PLANE_RENDERER_H
#define RENDERER_PLANE_RENDERER_H
#include "renderer.h"

namespace cg {
    class plane_renderer : public renderer {

    public:
        [[nodiscard]] bool Intersects(const ray &ray, glm::vec3 &intersection, glm::vec3 &normal, float& raycast_distance, float max_distance) const override;
    };
}


#endif //RENDERER_PLANE_RENDERER_H
