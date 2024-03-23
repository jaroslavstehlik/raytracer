//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "scene.h"

void cg::scene::AddRenderer(const std::shared_ptr<renderer>& renderer) {
    renderers_.emplace_back(renderer);
}

void cg::scene::AddLight(const std::shared_ptr<light>& light) {
    lights_.emplace_back(light);
}

std::span<const std::shared_ptr<cg::renderer>> cg::scene::GetRenderers() const {
    return std::span{renderers_};
}

std::span<const std::shared_ptr<cg::light>> cg::scene::GetLights() const {
    return std::span{lights_};
}
