//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H
#include "ray.h"
#include "transform.h"

namespace cg {
    class renderer {
    protected:
        transform transform_;
    public:
        void SetTransform(const transform &transform);

        const transform &GetTransform() const;

        [[nodiscard]] virtual bool
        Intersects(const ray &ray, glm::vec3 &intersection, glm::vec3 &normal, float& raycast_distance, float max_distance) const;
    };
}

#endif //RENDERER_RENDERER_H
