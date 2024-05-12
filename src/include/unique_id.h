//
// Created by Jaroslav Stehlik on 08.05.2024.
//
#pragma once
#include <cstdio>
#include <atomic>

struct unique_id
{
    static std::atomic<size_t> ID;
};