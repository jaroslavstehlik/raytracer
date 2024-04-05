//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#pragma once
#include <memory>
#include "renderer.h"
#include "mesh.h"

namespace cg {
    class mesh_renderer : public renderer {
    private:
        std::shared_ptr<cg::mesh> mesh_;

    public:
        void SetMesh(const std::shared_ptr<cg::mesh>& mesh);
        [[nodiscard]] bool Intersects(const ray &ray, glm::vec3 &intersection, glm::vec3 &normal, float& raycast_distance, float max_distance) const override;
    };
}
