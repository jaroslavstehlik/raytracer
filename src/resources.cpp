//
// Created by Jaroslav Stehlik on 03.04.2024.
//

#include "resources.h"
namespace cg {
    void resources::AddMesh(const std::string& file_name, const std::shared_ptr<cg::mesh>& mesh) {
        meshes_[file_name] = mesh;
    }

    const std::shared_ptr<cg::mesh>& resources::GetMesh(const std::string& mesh_name) const
    {
        if(!meshes_.contains(mesh_name))
            return nullptr;

        return meshes_.at(mesh_name);
    }
}