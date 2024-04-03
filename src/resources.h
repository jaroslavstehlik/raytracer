//
// Created by Jaroslav Stehlik on 03.04.2024.
//

#pragma once
#include <map>
#include <string>
#include <memory>
#include "mesh.h"

namespace cg {
    class resources {
    private:
        std::map<std::string, std::shared_ptr<cg::mesh>> meshes_{};
    public:
        void AddMesh(const std::string& file_name, const std::shared_ptr<cg::mesh>& mesh);
        const std::shared_ptr<cg::mesh>& GetMesh(const std::string& mesh_name) const;
    };
}
