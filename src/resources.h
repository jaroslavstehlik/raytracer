//
// Created by Jaroslav Stehlik on 03.04.2024.
//

#pragma once
#include <map>
#include <string>
#include <memory>
#include "mesh.h"
#include "materials/material.h"

namespace cg {
    class resources {
    private:
        std::map<std::string, std::shared_ptr<cg::mesh>> meshes_{};
        std::map<std::string, std::shared_ptr<cg::material>> materials_{};
    public:
        void AddMesh(const std::string& name, const std::shared_ptr<cg::mesh>& mesh);
        const std::shared_ptr<cg::mesh>& GetMesh(const std::string& name) const;

        void AddMaterial(const std::string& name, const std::shared_ptr<cg::material>& mesh);
        const std::shared_ptr<cg::material>& GetMaterial(const std::string& name) const;
    };
}
