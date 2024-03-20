//
// Created by Jaroslav Stehlik on 18.03.2024.
//
#include "renderer.h"

void renderer::SetTransform(const transform &transform) {
    transform_ = transform;
}

const transform &renderer::GetTransform() const {
    return transform_;
}

bool renderer::Intersects(const ray &ray, float raycast_distance) const {
    return false;
}
