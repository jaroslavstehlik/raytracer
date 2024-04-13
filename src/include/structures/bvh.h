#include <vector>
#include <span>
#include <string>
#include "glm/glm.hpp"
#include "algorithm"
#include "ray.h"
#include "AABB.h"

namespace cg
{
    // minimal structs
    struct Tri {
        glm::vec3 vertex0, vertex1, vertex2;
        glm::vec3 centroid;
    };

    struct BVHNode {
        cg::AABB bounds;
        uint32_t leftFirst, triCount;
        bool isLeaf() const { return triCount > 0; }
    };

    class bvh {
    private:
        // application data
        std::vector<Tri> triangles{};
        std::vector<uint32_t> triangle_indexes{};
        std::vector<BVHNode> bvh_nodes{};
        uint32_t rootNodeIdx = 0, nodesUsed = 1;

        bool IntersectTriangle(const cg::ray& ray,
                                    const glm::vec3& vertex0,
                                    const glm::vec3& vertex1,
                                    const glm::vec3& vertex2,
                                    float& raycast_distance) const;

        void IntersectBVH(const cg::ray& ray, const uint32_t nodeIdx, float& raycast_distance, uint32_t& out_nodeIdx) const;
        void UpdateNodeBounds(uint32_t nodeIdx);
        void Subdivide(uint32_t nodeIdx);

    public:
        void Build(const std::span<const glm::vec3>& positions, const std::span<const uint16_t>& indexes);
        bool Intersect(const cg::ray& ray, glm::vec3& normal, float& raycast_distance) const;
        std::string Debug() const;
    };
}