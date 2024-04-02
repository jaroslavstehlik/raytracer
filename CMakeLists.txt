cmake_minimum_required(VERSION 3.27)
project(renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#GLM
include(FetchContent)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG		   1.0.1
)
FetchContent_GetProperties(glm)
FetchContent_MakeAvailable(glm)

#STB
include(FetchContent)
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_GetProperties(stb)
FetchContent_MakeAvailable(stb)

#Nlohman json
include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG		   v3.11.3
)
FetchContent_GetProperties(json)
FetchContent_MakeAvailable(json)

#TinyGLTF
include(FetchContent)
FetchContent_Declare(
        tinygltf
        GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
        GIT_TAG		   v2.8.21
)

FetchContent_GetProperties(tinygltf)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)

FetchContent_MakeAvailable(tinygltf)
#add_subdirectory(${tinygltf_SOURCE_DIR})

include_directories(renderer PRIVATE src/include)

# renderer
add_executable(renderer "src/main.cpp"
        src/camera.cpp
        src/camera.h
        src/raytracer.cpp
        src/raytracer.h
        src/scene.cpp
        src/scene.h
        src/renderers/mesh_renderer.cpp
        src/renderers/mesh_renderer.h
        src/renderers/sphere_renderer.cpp
        src/renderers/sphere_renderer.h
        src/renderers/renderer.h
        src/include/ray.h
        src/include/transform.h
        src/renderers/renderer.cpp
        src/include/transform.cpp
        src/lights/light.cpp
        src/lights/light.h
        src/lights/point_light.cpp
        src/lights/point_light.h
        src/renderers/plane_renderer.cpp
        src/renderers/plane_renderer.h
        src/include/global_settings.h
        src/loaders/gltf_loader.cpp
        src/loaders/gltf_loader.h
        src/main.h
)

target_include_directories(renderer PRIVATE
        ${glm_SOURCE_DIR}
        ${stb_SOURCE_DIR})

target_link_libraries(renderer PRIVATE tinygltf)