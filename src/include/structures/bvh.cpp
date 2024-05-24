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

    void bvh::IntersectBVH(const std::span<const glm::vec3>& positions,
                           const cg::ray &ray,
                           const uint32_t nodeIdx,
                           float& raycast_distance,
                           glm::vec3& barycentric_coords,
                           uint32_t& out_nodeIdx) const {

        const BVHNode &node = bvh_nodes[nodeIdx];
        if (!node.bounds.Intersects(ray, raycast_distance)) return;
        if (node.isLeaf()) {
            for (uint32_t i = 0; i < node.triCount; i++) {
                int triangleIndex = triangle_indexes[node.leftFirst + i];
                float local_distance = 0;
                glm::vec3 local_barycentric_coords{};
                const cg::Tri& tri = triangles[triangleIndex];
                if(cg::IntersectTriangle(
                        ray,
                        positions[tri.vertex0],
                        positions[tri.vertex1],
                        positions[tri.vertex2],
                        local_distance,
                        local_barycentric_coords))
                {
                    if(local_distance < raycast_distance) {
                        raycast_distance = local_distance;
                        barycentric_coords = local_barycentric_coords;
                        out_nodeIdx = triangleIndex;
                    }
                }
            }
        } else {
            IntersectBVH(positions, ray, node.leftFirst, raycast_distance, barycentric_coords, out_nodeIdx);
            IntersectBVH(positions, ray, node.leftFirst + 1, raycast_distance, barycentric_coords, out_nodeIdx);
        }
    }

    bool bvh::Intersect(const std::span<const glm::vec3>& positions,
                        const cg::ray& ray,
                        Tri& triangle,
                        float& raycast_distance,
                        glm::vec3& barycentric_coords) const {
        uint32_t idx = -1;
        IntersectBVH(positions, ray, 0, raycast_distance, barycentric_coords, idx);
        if(idx == -1)
            return false;

        triangle = triangles[idx];
        return true;
    }

    void bvh::Build(const std::span<const glm::vec3>& positions,
                    const std::span<const uint16_t>& indexes) {
        const int triangle_count = indexes.size() / 3;
        triangles.resize(triangle_count);

        for(int i = 0; i < triangle_count; i++)
        {
            const int ti = i * 3;

            const glm::vec3& v0 = positions[indexes[ti]];
            const glm::vec3& v1 = positions[indexes[ti + 1]];
            const glm::vec3& v2 = positions[indexes[ti + 2]];

            triangles[i] = {indexes[ti], indexes[ti + 1], indexes[ti + 2],
                            (v0 + v1 + v2) / 3.f};
        }

        triangle_indexes.resize(triangle_count);
        bvh_nodes.resize(triangle_count * 2);

        // populate triangle index array
        // TODO: replace with std::algorithm
        for (int i = 0; i < triangle_count; i++) {
            triangle_indexes[i] = i;
        }

        // assign all triangles to root node
        BVHNode &root = bvh_nodes[rootNodeIdx];
        root.leftFirst = 0, root.triCount = triangle_count;
        UpdateNodeBounds(positions, rootNodeIdx);
        // subdivide recursively
        Subdivide(positions, rootNodeIdx);
    }

    void bvh::UpdateNodeBounds(const std::span<const glm::vec3>& positions, uint32_t nodeIdx) {
        BVHNode &node = bvh_nodes[nodeIdx];
        node.bounds.min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        node.bounds.max = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
        for (uint32_t first = node.leftFirst, i = 0; i < node.triCount; i++) {
            const uint32_t leafTriIdx = triangle_indexes[first + i];
            const Tri &leafTri = triangles[leafTriIdx];
            node.bounds.Expand(positions[leafTri.vertex0]);
            node.bounds.Expand(positions[leafTri.vertex1]);
            node.bounds.Expand(positions[leafTri.vertex2]);
        }
    }

    void bvh::Subdivide(const std::span<const glm::vec3>& positions, uint32_t nodeIdx) {
        // terminate recursion
        BVHNode &node = bvh_nodes[nodeIdx];
        if (node.triCount <= 2)
            return;
        // determine split axis and position
        const glm::vec3& extent = node.bounds.Extent();
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
        const int leftCount = i - node.leftFirst;
        if (leftCount == 0 || leftCount == node.triCount)
            return;
        // create child nodes
        const int leftChildIdx = nodesUsed++;
        const int rightChildIdx = nodesUsed++;
        bvh_nodes[leftChildIdx].leftFirst = node.leftFirst;
        bvh_nodes[leftChildIdx].triCount = leftCount;
        bvh_nodes[rightChildIdx].leftFirst = i;
        bvh_nodes[rightChildIdx].triCount = node.triCount - leftCount;
        node.leftFirst = leftChildIdx;
        node.triCount = 0;
        UpdateNodeBounds(positions, leftChildIdx);
        UpdateNodeBounds(positions, rightChildIdx);
        // recurse
        Subdivide(positions, leftChildIdx);
        Subdivide(positions, rightChildIdx);
    }

    std::string bvh::Debug(const std::span<const glm::vec3>& positions) const
    {
        std::string output{};
        for(const BVHNode& node : bvh_nodes)
        {
            output += std::to_string(node.bounds.min.x) + ";";
            output += std::to_string(node.bounds.min.y) + ";";
            output += std::to_string(node.bounds.min.z) + ";";
            output += std::to_string(node.bounds.max.x) + ";";
            output += std::to_string(node.bounds.max.y) + ";";
            output += std::to_string(node.bounds.max.z) + "#";
            if(node.isLeaf())
            {
                for(int i = 0; i < node.triCount; i++)
                {
                    const uint32_t leafTriIdx = triangle_indexes[node.leftFirst + i];
                    const Tri &leafTri = triangles[leafTriIdx];
                    const glm::vec3 v0 = positions[leafTri.vertex0];
                    const glm::vec3 v1 = positions[leafTri.vertex1];
                    const glm::vec3 v2 = positions[leafTri.vertex2];

                    output += std::to_string(v0.x)+";"+std::to_string(v0.y)+";"+std::to_string(v0.z)+";";
                    output += std::to_string(v1.x)+";"+std::to_string(v1.y)+";"+std::to_string(v1.z)+";";
                    output += std::to_string(v2.x)+";"+std::to_string(v2.y)+";"+std::to_string(v2.z)+";";
                }
            }
            output += "\n";
        }
        return output;
    }
}