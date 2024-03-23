//
// Created by Jaroslav Stehlik on 23.03.2024.
//

#ifndef RENDERER_POINT_LIGHT_H
#define RENDERER_POINT_LIGHT_H
#include "light.h"

namespace cg {
    class point_light : public light {
        glm::vec3 color_;
        float fallof_;

    public:
        void SetColor(const glm::vec3& color);
        [[nodiscard]] glm::vec3 GetColor(const ray &ray_, const glm::vec3& normal) const override;
    };
}


#endif //RENDERER_POINT_LIGHT_H
