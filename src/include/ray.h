//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#ifndef RENDERER_RAY_H
#define RENDERER_RAY_H
#include <glm/vec3.hpp>

namespace cg {
    struct ray {
        glm::vec3 origin;
        glm::vec3 direction;
    };
}

#endif //RENDERER_RAY_H
