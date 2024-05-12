//
// Created by Jaroslav Stehlik on 03.04.2024.
//

#include "resources.h"
namespace cg {
    void resources::AddMesh(const std::string& name, const std::shared_ptr<cg::mesh>& mesh) {
        meshes_[name] = mesh;
    }

    const std::shared_ptr<cg::mesh>& resources::GetMesh(const std::string& name) const
    {
        if(!meshes_.contains(name))
            return nullptr;

        return meshes_.at(name);
    }

    void resources::AddMaterial(const std::string& file_name, const std::shared_ptr<cg::material>& material) {
        materials_[file_name] = material;
    }

    const std::shared_ptr<cg::material>& resources::GetMaterial(const std::string& name) const
    {
        if(!materials_.contains(name))
            return nullptr;

        return materials_.at(name);
    }

    void resources::AddTexture(const std::string& file_name, const std::shared_ptr<cg::texture>& texture) {
        textures_[file_name] = texture;
    }

    const std::shared_ptr<cg::texture>& resources::GetTexture(const std::string& name) const
    {
        if(!textures_.contains(name))
            return nullptr;

        return textures_.at(name);
    }
}