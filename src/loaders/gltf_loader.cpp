//
// Created by Jaroslav Stehlik on 02.04.2024.
//

#include "gltf_loader.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "json.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#include "tiny_gltf.h"

namespace cg {
    void CoutVector3(const char* name, const glm::vec3& pos)
    {
        std::cout << name << "x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
    }

    bool gltf_loader::LoadModel(const std::string& path) const
    {
        std::string err;
        std::string warn;

        tinygltf::Model model;
        tinygltf::TinyGLTF loader;

        bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
        if (!warn.empty()) {
            printf("Warn: %s\n", warn.c_str());
        }
        if (!err.empty()) {
            printf("ERR: %s\n", err.c_str());
        }
        if (!ret) {
            printf("Failed to load .glTF : %s\n", path.c_str());
            return false;
        }

        for(tinygltf::Mesh& mesh : model.meshes) {
            std::cout << "mesh: " << mesh.name << std::endl;
            std::cout << "primitives: " << std::to_string(mesh.primitives.size()) << std::endl;
            for(tinygltf::Primitive& primitive : mesh.primitives)
            {
                std::cout << "indicies: " << std::to_string(primitive.indices) << std::endl;
                if(primitive.indices != -1) {
                    const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
                    if(indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        const tinygltf::BufferView &indexBufferView = model.bufferViews[indexAccessor.bufferView];
                        const tinygltf::Buffer &indexBuffer = model.buffers[indexBufferView.buffer];
                        const u_short *index_start = reinterpret_cast<const u_short *>(&indexBuffer.data[
                                indexBufferView.byteOffset + indexAccessor.byteOffset]);

                        std::vector<u_short> indexes(index_start, index_start + indexAccessor.count);
                        std::cout << indexes.size() << std::endl;
                        for (u_short index: indexes) {
                            std::cout << index << std::endl;
                        }
                    }
                }

                std::cout << "primitive attributes: " << std::to_string(primitive.attributes.size()) <<  std::endl;
                for(const auto& [key, value] : primitive.attributes)
                {
                    std::cout << "key: " << key << ", value: " << value <<  std::endl;
                    if(key.compare("POSITION") == 0)
                    {
                        const tinygltf::Accessor &accessor = model.accessors[value];
                        if(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
                        {
                            std::cout << accessor.componentType << std::endl;
                            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                            const glm::vec3* position_start = reinterpret_cast<const glm::vec3*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                            std::vector<glm::vec3> positions(position_start, position_start + accessor.count);
                            std::cout << positions.size() << std::endl;
                            for(const glm::vec3& position : positions)
                            {
                                CoutVector3("position: ", position);
                            }
                        }
                    } else if(key.compare("NORMAL") == 0)
                    {
                        const tinygltf::Accessor &accessor = model.accessors[value];
                        if(accessor.type == TINYGLTF_TYPE_VEC3)
                        {

                        }
                    }
                }
            }
        }
        return true;
    }


} // cg