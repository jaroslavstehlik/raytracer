//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#ifndef RENDERER_SCENE_H
#define RENDERER_SCENE_H
#include <vector>
#include <span>
#include "renderers/renderer.h"
#include "lights/light.h"

namespace cg {
    class scene {
    private:
        std::vector<std::shared_ptr<renderer>> renderers_{};
        std::vector<std::shared_ptr<light>> lights_{};
    public:
        void AddRenderer(const std::shared_ptr<renderer> &renderer);

        void AddLight(const std::shared_ptr<light> &light);

        [[nodiscard]] std::span<const std::shared_ptr<renderer>> GetRenderers() const;

        [[nodiscard]] std::span<const std::shared_ptr<light>> GetLights() const;
    };
}
#endif //RENDERER_SCENE_H
