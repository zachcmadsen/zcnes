#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

struct CpuState {
    std::uint16_t pc;
    std::uint8_t s;
    std::uint8_t a;
    std::uint8_t x;
    std::uint8_t y;
    std::uint8_t p;
    std::vector<std::array<std::uint16_t, 2>> ram;
};

struct ProcessorTest {
    CpuState initial;
    CpuState final;
};

std::vector<ProcessorTest> loadTests(std::string_view filename);
