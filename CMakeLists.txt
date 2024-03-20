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
)

target_include_directories(renderer PRIVATE ${glm_SOURCE_DIR})