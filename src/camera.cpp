//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#define GLM_ENABLE_EXPERIMENTAL
#include "camera.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

const glm::vec3 &camera::GetPosition() const {
    return position_;
}

void camera::SetPosition(const glm::vec3 &position) {
    position_ = position;
}

void camera::SetRotation(const glm::vec3 &rotation) {
    rotation_ = rotation;
}

void camera::SetVerticalFov(float fov) {
    vertical_fov_ = fov;
}


float camera::GetVerticalFov() const {
    return vertical_fov_;
}

float camera::GetNearClipPlane() const {
    return near_clip_plane_;
}

void camera::SetNearClipPlane(float near_clip_plane) {
    near_clip_plane_ = near_clip_plane;
}

float camera::GetFarClipPlane() const {
    return far_clip_plane_;
}

void camera::SetFarClipPlane(float far_clip_plane) {
    far_clip_plane_ = far_clip_plane;
}

float camera::GetAspectRatio() const {
    return aspect_ratio_;
}

void camera::SetAspectRatio(float aspect_ratio) {
    aspect_ratio_ = aspect_ratio;
}

glm::mat4x4 camera::GetViewMatrix() const {

    glm::mat4x4 translate_matrix = glm::translate(
            glm::mat4x4(1.0),
            glm::vec3 (position_.x, position_.y, position_.z));

    // yaw, pitch, roll
    glm::mat4x4 rotation_matrix = glm::eulerAngleYXZ(
            glm::radians(rotation_.y),
            glm::radians(rotation_.x),
            glm::radians(rotation_.z));

    glm::mat4x4 scale_matrix = glm::scale(
            glm::mat4x4(1.0),
            glm::vec3 (
            1.0f,
            1.0f,
            -1.0f));

    return translate_matrix * rotation_matrix * scale_matrix;
}

glm::mat4x4 camera::GetProjectionMatrix() const {
    return glm::perspective(
            glm::radians(vertical_fov_),
            aspect_ratio_,
            near_clip_plane_,
            far_clip_plane_);
}
