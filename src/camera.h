//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#pragma once
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace cg {
    class camera {
        glm::vec3 position_;
        glm::vec3 rotation_;
        float vertical_fov_;
        float near_clip_plane_;
        float far_clip_plane_;
        float aspect_ratio_;
    public:
        const glm::vec3 &GetPosition() const;

        void SetPosition(const glm::vec3 &position);

        void SetRotation(const glm::vec3 &rotation);

        float GetVerticalFov() const;

        void SetVerticalFov(float fov);

        float GetNearClipPlane() const;

        void SetNearClipPlane(float near_clip_plane);

        float GetFarClipPlane() const;

        void SetFarClipPlane(float far_clip_plane);

        float GetAspectRatio() const;

        void SetAspectRatio(float aspect_ratio);

        [[nodiscard]] glm::mat4x4 GetViewMatrix() const;

        [[nodiscard]] glm::mat4x4 GetProjectionMatrix() const;
    };
}
