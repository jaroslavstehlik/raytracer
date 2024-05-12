//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#pragma once
#include <memory>
#include "renderer.h"
#include "mesh.h"
#include "structures/bvh.h"

namespace cg {
    class mesh_renderer : public renderer {
    private:
        std::shared_ptr<cg::mesh> mesh_;
        cg::bvh bvh{};
        std::vector<glm::vec3> world_space_positions_;
        std::vector<glm::vec3> world_space_normals_;

    public:
        void SetMesh(const std::shared_ptr<cg::mesh>& mesh);
        [[nodiscard]] bool Intersects(const ray &ray,
                                      glm::vec3 &intersection,
                                      glm::vec3 &normal,
                                      float& raycast_distance,
                                      glm::vec3& barycentric_coords,
                                      glm::vec2& uv,
                                      float max_distance) const override;
        void Prepass() override;
    };
}
