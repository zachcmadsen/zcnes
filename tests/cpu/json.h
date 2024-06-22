#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

using RamState = std::tuple<std::uint16_t, std::uint8_t>;

using BusState = std::tuple<std::uint16_t, std::uint8_t, std::string>;

struct CpuState
{
    std::uint16_t pc;
    std::uint8_t s;
    std::uint8_t a;
    std::uint8_t x;
    std::uint8_t y;
    std::uint8_t p;
    std::vector<RamState> ram;
};

struct ProcessorTest
{
    CpuState initial;
    CpuState final;
    std::vector<BusState> cycles;
};

std::vector<ProcessorTest> LoadTests(std::uint8_t opcode);
