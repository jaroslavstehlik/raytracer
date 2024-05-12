//
// Created by Jaroslav Stehlik on 22.04.2024.
//
#pragma once
#include <vector>
#include <span>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace cg {
    class texture {
    private:
        int32_t width_;
        int32_t height_;
        int32_t components_;
        std::vector<unsigned char> colors_;
    public:
        texture(int32_t width, int32_t height, int32_t components, const std::vector<unsigned char>& colors)
            : width_(width)
            , height_(height)
            , components_(components)
            , colors_(colors) {}

        int32_t Width() const {
            return width_;
        }

        int32_t Height() const {
            return height_;
        }

        glm::vec4 SampleColor(const glm::vec2& uv) const {
            //return {uv.x, uv.y, 0.f, 0.f};
            uint32_t row = uv.x * (float)width_;
            uint32_t collumn = uv.y * (float)height_;

            int32_t i = (collumn * width_ + row) * components_;
            switch (components_) {
                case 1:
                    return {(float)colors_[i] / 255.f, 0.f, 0.f, 1.f};
                case 2:
                    return {(float)colors_[i] / 255.f, (float)colors_[i + 1] / 255.f, 0.f, 1.f};
                case 3:
                    return {(float)colors_[i] / 255.f, (float)colors_[i + 1] / 255.f, (float)colors_[i + 2] / 255.f, 1.f};
                case 4:
                    return {(float)colors_[i] / 255.f, (float)colors_[i + 1] / 255.f, (float)colors_[i + 2] / 255.f, (float)colors_[i + 3] / 255.f};
            }
            return {};
        }
    };
}