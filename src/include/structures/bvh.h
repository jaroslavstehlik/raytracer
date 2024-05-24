#include <vector>
#include <span>
#include <string>
#include "glm/glm.hpp"
#include "geom.h"
#include "algorithm"
#include "ray.h"
#include "AABB.h"

namespace cg
{
    // minimal structs
    struct Tri {
        glm::uint16_t vertex0, vertex1, vertex2{};
        glm::vec3 centroid{};
    };

    struct BVHNode {
        cg::AABB bounds{};
        uint32_t leftFirst, triCount{0};
        bool isLeaf() const { return triCount > 0; }
    };

    class bvh {
    private:
        // application data
        std::vector<Tri> triangles{};
        std::vector<uint32_t> triangle_indexes{};
        std::vector<BVHNode> bvh_nodes{};
        uint32_t rootNodeIdx = 0, nodesUsed = 1;

        void IntersectBVH(const std::span<const glm::vec3>& positions,
                          const cg::ray& ray,
                          const uint32_t nodeIdx,
                          float& raycast_distance,
                          glm::vec3& barycentric_coords,
                          uint32_t& out_nodeIdx) const;
        void UpdateNodeBounds(const std::span<const glm::vec3>& positions, uint32_t nodeIdx);
        void Subdivide(const std::span<const glm::vec3>& positions, uint32_t nodeIdx);

    public:
        void Build(const std::span<const glm::vec3>& positions, const std::span<const uint16_t>& indexes);
        bool Intersect(const std::span<const glm::vec3>& positions,
                        const cg::ray& ray,
                            Tri& triangle,
                            float& raycast_distance,
                            glm::vec3& barycentric_coords) const;
        std::string Debug(const std::span<const glm::vec3>& positions) const;
    };
}