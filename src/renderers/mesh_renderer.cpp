//
// Created by Jaroslav Stehlik on 18.03.2024.
//

#include "mesh_renderer.h"
#include "global_settings.h"

void cg::mesh_renderer::SetMesh(const std::shared_ptr<cg::mesh> &mesh) {
    mesh_ = mesh;
}

bool cg::mesh_renderer::Intersects(const ray &ray, glm::vec3& intersection, glm::vec3& normal, float& raycast_distance, float max_distance) const {
    if(!mesh_)
        return false;

    if(!bounds_.Intersects(ray))
        return false;

    // TODO: check if mesh is valid
    const std::span<const uint16_t> indexes = mesh_->GetIndexes();
    const std::span<const glm::vec3> positions = mesh_->GetPositions();

    const glm::mat4x4 objectToWorldMatrix = transform_.ObjectToWorldMatrix();
    float nearest_raycast_distance = std::numeric_limits<float>::max();

    // step each triangle
    for(int i = 0; i < indexes.size(); i += 3)
    {
        // obtain model space triangle
        glm::vec3 v0 = positions[indexes[i + 0]];
        glm::vec3 v1 = positions[indexes[i + 1]];
        glm::vec3 v2 = positions[indexes[i + 2]];

        // convert triangle to world space
        v0 = objectToWorldMatrix * glm::vec4(v0.x, v0.y, v0.z, 1.f);
        v1 = objectToWorldMatrix * glm::vec4(v1.x, v1.y, v1.z, 1.f);
        v2 = objectToWorldMatrix * glm::vec4(v2.x, v2.y, v2.z, 1.f);

        // intersect plane of the triangle
        const glm::vec3 local_normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        const float dot_product = glm::dot(ray.direction, local_normal);
        if(abs(dot_product) < cg::kEpsilon)
            continue;

        const glm::vec3 local_position = v0 - ray.origin;
        const float local_raycast_distance = glm::dot(local_position, local_normal) / dot_product;
        if(local_raycast_distance < 0.f)
            continue;

        const glm::vec3 local_intersection = ray.origin + ray.direction * local_raycast_distance;

        // Inside-Outside Test
        // Step 2: Inside-Outside Test
        glm::vec3 C; // Vector perpendicular to triangle's plane

        // Edge 0
        glm::vec3 edge0 = v1 - v0;
        glm::vec3 vp0 = local_intersection - v0;
        C = glm::cross(edge0, vp0);
        if (glm::dot(local_normal, C) < 0)
            continue; // P is on the right side

        // Edge 1
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 vp1 = local_intersection - v1;
        C = glm::cross(edge1, vp1);
        if (glm::dot(local_normal, C) < 0)
            continue;  // P is on the right side

        // Edge 2
        glm::vec3 edge2 = v0 - v2;
        glm::vec3 vp2 = local_intersection - v2;
        C = glm::cross(edge2, vp2);
        if (glm::dot(local_normal, C) < 0)
            continue; // P is on the right side

        // This ray hits the triangle
        if(local_raycast_distance < nearest_raycast_distance)
        {
            nearest_raycast_distance = local_raycast_distance;
            raycast_distance = nearest_raycast_distance;
            intersection = local_intersection;
            normal = local_normal;
        }
    }

    if(nearest_raycast_distance == std::numeric_limits<float>::max())
        return false;

    // apply shadow bias to prevent shadow acne
    raycast_distance -= cg::kEpsilon;
    intersection = ray.origin + ray.direction * raycast_distance;
    return true;
}

void cg::mesh_renderer::RecalculateBounds() {

    const std::span<const uint16_t> indexes = mesh_->GetIndexes();
    if(indexes.size() == 0)
    {
        bounds_.Reset();
        return;
    }

    const std::span<const glm::vec3> positions = mesh_->GetPositions();

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
    }
}
