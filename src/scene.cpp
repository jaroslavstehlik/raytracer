//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "scene.h"

std::span<const std::shared_ptr<renderer>> scene::GetRenderers() const {
    return std::span{renderers_};
}
