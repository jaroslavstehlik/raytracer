//
// Created by Jaroslav Stehlik on 03.04.2024.
//
#pragma once
#include <vector>
#include <span>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace cg {
    class mesh {
    private:
        std::vector<glm::vec3> positions_;
        std::vector<uint16_t> indexes_;
        std::vector<glm::vec2> texcoord0_;
    public:
        std::span<const glm::vec3> GetPositions() const {
            return {positions_};
        }
        void SetPositions(const glm::vec3* begin, size_t length) {
            positions_ = {begin, begin + length};
        }
        std::span<const uint16_t> GetIndexes() const {
            return {indexes_};
        }
        void SetIndexes(const uint16_t* begin, size_t length) {
            indexes_ = {begin, begin + length};
        }
        std::span<const glm::vec2> GetTexcoord0() const {
            return {texcoord0_};
        }
        void SetTexcoord0(const glm::vec2* begin, size_t length) {
            texcoord0_ = {begin, begin + length};
        }

        // todo: add mesh validation
    };
}