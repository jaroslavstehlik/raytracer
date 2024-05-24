//
// Created by Jaroslav Stehlik on 18.03.2024.
//
#define BVH_ACCELERATION

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
                                   glm::vec3& barycentric_coords,
                                   glm::vec2& uv,
                                   float max_distance) const {
    if(!mesh_)
        return false;

    if(!bounds_.Intersects(ray, max_distance))
        return false;

    float nearest_raycast_distance = std::numeric_limits<float>::max();

    Tri tri{};

#if !defined(BVH_ACCELERATION)
    const auto indexes = mesh_->GetIndexes();
    for(int i = 0; i < indexes.size(); i+=3)
    {
        const glm::vec3& v0 = world_space_positions_[indexes[i + 0]];
        const glm::vec3& v1 = world_space_positions_[indexes[i + 1]];
        const glm::vec3& v2 = world_space_positions_[indexes[i + 2]];

        float local_raycast_distance;
        glm::vec3 local_barycentric_coords{};
        if(cg::IntersectTriangle(ray, v0, v1, v2, local_raycast_distance, local_barycentric_coords))
        {
            if(local_raycast_distance >= nearest_raycast_distance)
                continue;

            barycentric_coords = local_barycentric_coords;
            nearest_raycast_distance = local_raycast_distance;

            tri.vertex0 = indexes[i + 0];
            tri.vertex1 = indexes[i + 1];
            tri.vertex2 = indexes[i + 2];
            tri.centroid = (v0 + v1 + v2) / 3.f;
        }
    }
#else
    if(!bvh.Intersect(world_space_positions_, ray, tri, nearest_raycast_distance, barycentric_coords))
        return false;
#endif

    if(nearest_raycast_distance == std::numeric_limits<float>::max())
        return false;

    if(mesh_->IsNormalsValid()) {
        normal = glm::normalize(cg::BarycentricInterpolate(
                world_space_normals_[tri.vertex1],
                world_space_normals_[tri.vertex2],
                world_space_normals_[tri.vertex0],
                barycentric_coords));
    }

    if(mesh_->IsTexcoord0Valid()) {
        const auto uvs = mesh_->GetTexcoord0();

        uv = cg::BarycentricInterpolate(
                uvs[tri.vertex1],
                uvs[tri.vertex2],
                uvs[tri.vertex0],
                barycentric_coords);

        // flip Y
        uv.y = 1.f - uv.y;
    }

    raycast_distance = nearest_raycast_distance;
    // apply shadow bias to prevent shadow acne
    raycast_distance -= cg::kEpsilon;
    intersection = ray.origin + ray.direction * raycast_distance;
    return true;
}

void cg::mesh_renderer::Prepass() {
    mesh_->Validate();

    const std::span<const uint16_t> indexes = mesh_->GetIndexes();
    if(indexes.size() == 0)
    {
        bounds_.Reset();
        return;
    }

    const std::span<const glm::vec3> positions = mesh_->GetPositions();
    const std::span<const glm::vec3> normals = mesh_->GetNormals();
    world_space_positions_.resize(positions.size());
    world_space_normals_.resize(normals.size());

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

        // imported normals
        const glm::vec3 n0 = normals[indexes[i + 0]];
        const glm::vec3 n1 = normals[indexes[i + 1]];
        const glm::vec3 n2 = normals[indexes[i + 2]];

        // calculated normals
        /*
        glm::vec3 d1 = positions[indexes[i + 1]] - positions[indexes[i + 0]];
        glm::vec3 d2 = positions[indexes[i + 2]] - positions[indexes[i + 0]];
        glm::vec3 normal = glm::normalize(glm::cross(d2, d1));
        const glm::vec3 n0 = normal;
        const glm::vec3 n1 = normal;
        const glm::vec3 n2 = normal;
        */

        world_space_normals_[indexes[i + 0]] = objectToWorldMatrix * glm::vec4(n0.x, n0.y, n0.z, 0.f);
        world_space_normals_[indexes[i + 1]] = objectToWorldMatrix * glm::vec4(n1.x, n1.y, n1.z, 0.f);
        world_space_normals_[indexes[i + 2]] = objectToWorldMatrix * glm::vec4(n2.x, n2.y, n2.z, 0.f);
    }

    bvh.Build(world_space_positions_, indexes);

    // Debug bounds
    std::ofstream myfile;
    myfile.open ("debug_bounds.txt");
    myfile << bvh.Debug(world_space_positions_);
    myfile.close();
    std::cout << "wrote debug bounds" << std::endl;

    // Debug triangles
    myfile.open("debug_triangles.txt");
    for(int i = 0; i < indexes.size(); i += 3)
    {
        const glm::vec3 v0 = world_space_positions_[indexes[i]];
        const glm::vec3 v1 = world_space_positions_[indexes[i + 1]];
        const glm::vec3 v2 = world_space_positions_[indexes[i + 2]];

        std::string triangle = std::to_string(v0.x)+";"+std::to_string(v0.y)+";"+std::to_string(v0.z)+";";
        triangle += std::to_string(v1.x)+";"+std::to_string(v1.y)+";"+std::to_string(v1.z)+";";
        triangle += std::to_string(v2.x)+";"+std::to_string(v2.y)+";"+std::to_string(v2.z)+";";
        myfile << triangle + "\n";
    }
    myfile << "";
    myfile.close();
    std::cout << "wrote debug triangles" << std::endl;
}
