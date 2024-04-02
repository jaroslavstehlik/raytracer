//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cg {
    struct transform {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4x4 TranslationMatrix() const;
        glm::mat4x4 RotationMatrix() const;
        glm::mat4x4 ScaleMatrix() const;
        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;
        glm::mat4x4 ObjectToWorldMatrix() const;
        glm::mat4x4 WorldToObjectMatrix() const;
    };
}