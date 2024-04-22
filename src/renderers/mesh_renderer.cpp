//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "mesh_renderer.h"
#include "global_settings.h"
#include <iostream>
#include <fstream>

void cg::mesh_renderer::SetMesh(const std::shared_ptr<cg::mesh> &mesh) {
    mesh_ = mesh;
}

bool cg::mesh_renderer::Intersects(const ray &ray,
                                   glm::vec3& intersection,
                                   glm::vec3& normal,
                                   float& raycast_distance,
                                   glm::vec2& uv,
                                   float max_distance) const {
    if(!mesh_)
        return false;

    if(!bounds_.Intersects(ray, max_distance))
        return false;

    float nearest_raycast_distance = std::numeric_limits<float>::max();
    if(!bvh.Intersect(ray, normal, nearest_raycast_distance, uv))
        return false;

    if(nearest_raycast_distance == std::numeric_limits<float>::max())
        return false;

    raycast_distance = nearest_raycast_distance;
    // apply shadow bias to prevent shadow acne
    raycast_distance -= cg::kEpsilon;
    intersection = ray.origin + ray.direction * raycast_distance;
    //normal = glm::normalize(glm::cross(fv1 - fv0, fv2 - fv0));
    return true;
}

void cg::mesh_renderer::Prepass() {

    const std::span<const uint16_t> indexes = mesh_->GetIndexes();
    if(indexes.size() == 0)
    {
        bounds_.Reset();
        return;
    }

    const std::span<const glm::vec3> positions = mesh_->GetPositions();
    world_space_positions_.resize(positions.size());
    world_space_normals_.resize(positions.size());

    const glm::mat4x4 objectToWorldMatrix = transform_.ObjectToWorldMatrix();

    bounds_.min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    bounds_.max = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    // step each triangle
    for(int i = 0; i < indexes.size(); i += 3) {
        // obtain model space triangle
        glm::vec3 v0 = positions[indexes[i + 0]];
        glm::vec3 v1 = positions[indexes[i + 1]];
        glm::vec3 v2 = positions[indexes[i + 2]];

        // convert triangle to world space
        v0 = objectToWorldMatrix * glm::vec4(v0.x, v0.y, v0.z, 1.f);
        v1 = objectToWorldMatrix * glm::vec4(v1.x, v1.y, v1.z, 1.f);
        v2 = objectToWorldMatrix * glm::vec4(v2.x, v2.y, v2.z, 1.f);

        bounds_.Expand(v0);
        bounds_.Expand(v1);
        bounds_.Expand(v2);

        world_space_positions_[indexes[i + 0]] = v0;
        world_space_positions_[indexes[i + 1]] = v1;
        world_space_positions_[indexes[i + 2]] = v2;

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        world_space_normals_[indexes[i + 0]] = normal;
        world_space_normals_[indexes[i + 1]] = normal;
        world_space_normals_[indexes[i + 2]] = normal;
    }

    bvh.Build(world_space_positions_, indexes);

    /*
    std::ofstream myfile;
    myfile.open ("debug_bounds.txt");
    myfile << bvh.Debug();
    myfile.close();
    std::cout << "wrote debug" << std::endl;
     */
}
