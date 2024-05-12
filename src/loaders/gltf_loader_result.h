//
// Created by Jaroslav Stehlik on 08.05.2024.
//

#pragma once
#include <vector>
#include <string>

namespace cg {
    struct gltf_loader_result {
        bool success;
        std::vector<std::string> mesh_names;
        std::vector<std::string> texture_names;
        std::vector<std::string> material_names;
    };
}