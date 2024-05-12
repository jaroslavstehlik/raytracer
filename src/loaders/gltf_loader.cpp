//
// Created by Jaroslav Stehlik on 02.04.2024.
//

#include "gltf_loader.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "texture.h"
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

    // output mesh, material, textures
    // mesh: vertices, uvs, triangles
    // material: texture_ids
    // textures: texture_ids
    gltf_loader_result gltf_loader::LoadModel(const std::string& path, cg::resources& resources) const
    {
        gltf_loader_result result{false};

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
            result;
        }

        std::cout << "texture size: " << model.textures.size() << std::endl;
        for(tinygltf::Texture& texture : model.textures) {
            std::string texture_name = path +"."+texture.name;
            std::cout << "texture: " << texture_name << std::endl;
            const tinygltf::Image& image = model.images[texture.source];

            resources.AddTexture(texture_name, std::make_shared<cg::texture>(image.width, image.height, image.component, image.image));
            result.texture_names.push_back(texture_name);
        }

        for(tinygltf::Material& material : model.materials) {
            std::cout << "material: " << material.name << std::endl;

            std::string base_color_texture_name{};
            glm::vec4 albedo_color = {1, 1, 1, 1};
            float metallic = 0;
            float rougness = 0;
            for(const auto& key_value : material.values)
            {
                if(key_value.first == "baseColorTexture")
                {
                    base_color_texture_name = path+"."+key_value.second.string_value;
                }
                if(key_value.first == "metallicFactor")
                {
                    if(key_value.second.has_number_value)
                        metallic = key_value.second.number_value;
                }
                if(key_value.first == "roughnessFactor")
                {
                    if(key_value.second.has_number_value)
                        rougness = key_value.second.number_value;
                }

                std::cout << key_value.first << std::endl;
                std::cout << key_value.second.string_value << std::endl;
            }

            printf("base_color_texture_name: %s\n", base_color_texture_name.c_str());
            printf("albedo_color: %.3f %.3f %.3f %.3f\n", std::to_string(albedo_color.x).c_str(),
                   std::to_string(albedo_color.y).c_str(),
                   std::to_string(albedo_color.z).c_str(),
                   std::to_string(albedo_color.w).c_str());

            printf("metallic: %s\n", std::to_string(metallic).c_str());
            printf("rougness: %s\n", std::to_string(rougness).c_str());
            std::shared_ptr<cg::material> out_material = std::make_shared<cg::material>(resources.GetTexture(base_color_texture_name), albedo_color, metallic, rougness);
            std::string material_name = path+"."+material.name;
            resources.AddMaterial(material_name, out_material);
            result.material_names.push_back(material_name);
        }

        for(tinygltf::Mesh& mesh : model.meshes) {
            std::shared_ptr<cg::mesh> out_mesh = std::make_shared<cg::mesh>();

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
                        const uint16_t *index_start = reinterpret_cast<const uint16_t*>(&indexBuffer.data[
                                indexBufferView.byteOffset + indexAccessor.byteOffset]);

                        out_mesh->SetIndexes(index_start, indexAccessor.count);
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
                            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                            const glm::vec3* position_start = reinterpret_cast<const glm::vec3*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                            out_mesh->SetPositions(position_start, accessor.count);
                        }
                    } else if(key.compare("TEXCOORD_0") == 0)
                    {
                        const tinygltf::Accessor &accessor = model.accessors[value];
                        if(accessor.type == TINYGLTF_TYPE_VEC2 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
                        {
                            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                            const glm::vec2* texcoord0_start = reinterpret_cast<const glm::vec2*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                            out_mesh->SetTexcoord0(texcoord0_start, accessor.count);
                        }
                    } else if(key.compare("NORMAL") == 0)
                    {
                        const tinygltf::Accessor &accessor = model.accessors[value];
                        if(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
                        {
                            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                            const glm::vec3* normals_start = reinterpret_cast<const glm::vec3*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                            out_mesh->SetNormals(normals_start, accessor.count);
                        }
                    }
                }
            }

            std::string mesh_name = path+"."+mesh.name;
            resources.AddMesh(mesh_name, out_mesh);
            result.mesh_names.push_back(mesh_name);
        }

        result.success = true;
        return result;
    }


} // cg