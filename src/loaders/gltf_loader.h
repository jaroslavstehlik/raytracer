//
// Created by Jaroslav Stehlik on 02.04.2024.
//

#pragma once
#include <string>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cg {
    class gltf_loader {
    public:
        bool LoadModel(const std::string& path) const;
    };

} // cg