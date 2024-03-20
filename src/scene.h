//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#ifndef RENDERER_SCENE_H
#define RENDERER_SCENE_H
#include <vector>
#include <span>
#include "renderers/renderer.h"

class scene {
private:
    std::vector<std::shared_ptr<renderer>> renderers_{};
public:
    void AddRenderer(const std::shared_ptr<renderer>& renderer) {
        renderers_.emplace_back(renderer);
    }

    [[nodiscard]] std::span<const std::shared_ptr<renderer>> GetRenderers() const;
};

#endif //RENDERER_SCENE_H
