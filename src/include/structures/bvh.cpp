#include "bvh.h"
#include "iostream"

// https://developer.nvidia.com/blog/thinking-parallel-part-ii-tree-traversal-gpu/

namespace cg {
// THIS SOURCE FILE:
// Code for the article "How to Build a BVH", part 1: basics. Link:
// https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics
// This is bare-bones BVH construction and traversal code, running in
// a minimalistic framework.
// Feel free to copy this code to your own framework. Absolutely no
// rights are reserved. No responsibility is accepted either.
// For updates, follow me on twitter: @j_bikker.

    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    bool bvh::IntersectTriangle(const cg::ray& ray,
                           const glm::vec3& vertex0,
                           const glm::vec3& vertex1,
                           const glm::vec3& vertex2,
                           float& raycast_distance) const {

        constexpr float epsilon = std::numeric_limits<float>::epsilon();
        const glm::vec3 edge1 = vertex1 - vertex0;
        const glm::vec3 edge2 = vertex2 - vertex0;
        const glm::vec3 ray_cross_e2 = cross(ray.direction, edge2);
        const float det = dot(edge1, ray_cross_e2);
        if (det > -epsilon && det < epsilon)
            return false; // ray parallel to triangle

        const float inv_det = 1 / det;
        const glm::vec3 s = ray.origin - vertex0;
        const float u = inv_det * dot(s, ray_cross_e2);
        if (u < 0 || u > 1)
            return false;

        const glm::vec3 s_cross_e1 = cross(s, edge1);
        const float v = inv_det * dot(ray.direction, s_cross_e1);
        if (v < 0 || u + v > 1)
            return false;

        const float t = inv_det * dot(edge2, s_cross_e1);

        if (t <= epsilon)
            return false;

        raycast_distance = t;
        return true;
    }

    void bvh::IntersectBVH(const cg::ray &ray, const uint32_t nodeIdx, float& raycast_distance, uint32_t& out_nodeIdx) const {
        const BVHNode &node = bvh_nodes[nodeIdx];
        if (!node.bounds.Intersects(ray, raycast_distance)) return;
        if (node.isLeaf()) {
            for (uint32_t i = 0; i < node.triCount; i++) {
                int triangleIndex = triangle_indexes[node.leftFirst + i];
                float local_distance = 0;
                const cg::Tri& tri = triangles[triangleIndex];
                if(IntersectTriangle(ray, tri.vertex0, tri.vertex1, tri.vertex2, local_distance))
                {
                    if(local_distance < raycast_distance) {
                        raycast_distance = local_distance;
                        out_nodeIdx = triangleIndex;
                    }
                }
            }
        } else {
            IntersectBVH(ray, node.leftFirst, raycast_distance, out_nodeIdx);
            IntersectBVH(ray, node.leftFirst + 1, raycast_distance, out_nodeIdx);
        }
    }

    bool bvh::Intersect(const cg::ray& ray, glm::vec3& normal, float& raycast_distance) const {
        uint32_t idx = 0;
        IntersectBVH(ray, 0, raycast_distance, idx);
        if(idx == 0)
            return false;

        const Tri& t = triangles[idx];
        normal = glm::normalize(glm::cross(t.vertex1 - t.vertex0, t.vertex2 - t.vertex0));
        return true;
    }

    void bvh::Build(const std::span<const glm::vec3>& positions, const std::span<const uint16_t>& indexes) {
        const int triangle_count = indexes.size() / 3;
        triangles.resize(triangle_count);
        for(int i = 0; i < triangle_count; i++)
        {
            int ti = i * 3;
            triangles[i] = {positions[indexes[ti]], positions[indexes[ti + 1]], positions[indexes[ti + 2]]};
        }

        triangle_indexes.resize(triangle_count);
        bvh_nodes.resize(triangle_count * 2);

        // populate triangle index array
        for (int i = 0; i < triangle_count; i++) {
            triangle_indexes[i] = i;
        }
        // calculate triangle centroids for partitioning
        for (int i = 0; i < triangle_count; i++) {
            triangles[i].centroid = (triangles[i].vertex0 + triangles[i].vertex1 + triangles[i].vertex2) * 0.3333f;
        }
        // assign all triangles to root node
        BVHNode &root = bvh_nodes[rootNodeIdx];
        root.leftFirst = 0, root.triCount = triangle_count;
        UpdateNodeBounds(rootNodeIdx);
        // subdivide recursively
        Subdivide(rootNodeIdx);
    }

    void bvh::UpdateNodeBounds(uint32_t nodeIdx) {
        BVHNode &node = bvh_nodes[nodeIdx];
        node.bounds.min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        node.bounds.max = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
        for (uint32_t first = node.leftFirst, i = 0; i < node.triCount; i++) {
            uint32_t leafTriIdx = triangle_indexes[first + i];
            Tri &leafTri = triangles[leafTriIdx];

            node.bounds.Expand(leafTri.vertex0);
            node.bounds.Expand(leafTri.vertex1);
            node.bounds.Expand(leafTri.vertex2);
        }
    }

    void bvh::Subdivide(uint32_t nodeIdx) {
        // terminate recursion
        BVHNode &node = bvh_nodes[nodeIdx];
        if (node.triCount <= 2) return;
        // determine split axis and position
        glm::vec3 extent = node.bounds.Extent();
        int axis = 0;
        if (extent.y > extent.x) axis = 1;
        if (extent.z > extent[axis]) axis = 2;
        float splitPos = node.bounds.min[axis] + extent[axis] * 0.5f;
        // in-place partition
        int i = node.leftFirst;
        int j = i + node.triCount - 1;
        while (i <= j) {
            if (triangles[triangle_indexes[i]].centroid[axis] < splitPos) {
                i++;
            } else {
                std::swap(triangle_indexes[i], triangle_indexes[j--]);
            }
        }
        // abort split if one of the sides is empty
        int leftCount = i - node.leftFirst;
        if (leftCount == 0 || leftCount == node.triCount) return;
        // create child nodes
        int leftChildIdx = nodesUsed++;
        int rightChildIdx = nodesUsed++;
        bvh_nodes[leftChildIdx].leftFirst = node.leftFirst;
        bvh_nodes[leftChildIdx].triCount = leftCount;
        bvh_nodes[rightChildIdx].leftFirst = i;
        bvh_nodes[rightChildIdx].triCount = node.triCount - leftCount;
        node.leftFirst = leftChildIdx;
        node.triCount = 0;
        UpdateNodeBounds(leftChildIdx);
        UpdateNodeBounds(rightChildIdx);
        // recurse
        Subdivide(leftChildIdx);
        Subdivide(rightChildIdx);
    }

    std::string bvh::Debug() const
    {
        std::string output{};
        for(const BVHNode& node : bvh_nodes)
        {
            output += std::to_string(node.bounds.min.x) + ";";
            output += std::to_string(node.bounds.min.y) + ";";
            output += std::to_string(node.bounds.min.z) + ";";
            output += std::to_string(node.bounds.max.x) + ";";
            output += std::to_string(node.bounds.max.y) + ";";
            output += std::to_string(node.bounds.max.z);
            output += "\n";
        }
        return output;
    }
}