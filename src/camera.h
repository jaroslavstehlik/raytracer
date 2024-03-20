//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#ifndef RENDERER_CAMERA_H
#define RENDERER_CAMERA_H
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

class camera {
    glm::vec3 position_;
    glm::vec3 rotation_;
    float vertical_fov_;
    float near_clip_plane_;
    float far_clip_plane_;
    float aspect_ratio_;
public:
    const glm::vec3& GetPosition() const;
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetVerticalFov(float fov);
    void SetNearClipPlane(float near_clip_plane);
    void SetFarClipPlane(float far_clip_plane);
    void SetAspectRatio(float aspect_ratio);

    glm::mat4x4 GetViewMatrix();
    glm::mat4x4 GetProjectionMatrix();
};


#endif //RENDERER_CAMERA_H
