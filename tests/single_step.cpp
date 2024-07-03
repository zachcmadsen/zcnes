#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <span>

#include <cista/containers/string.h>
#include <cista/containers/vector.h>
#include <cista/mmap.h>
#include <cista/serialization.h>
#define ZCNES_SINGLE_STEP_TESTS
#include <cpu.hpp>

#include "util.hpp"

constexpr std::size_t addr_space_size = 0x10000;

struct RamState
{
    std::uint16_t addr;
    std::uint8_t data;
};

struct BusState
{
    std::uint16_t addr;
    std::uint8_t data;
    cista::offset::string kind;

    bool operator==(const BusState &) const = default;
};

struct CpuState
{
    std::uint16_t pc;
    std::uint8_t s;
    std::uint8_t a;
    std::uint8_t x;
    std::uint8_t y;
    std::uint8_t p;
    cista::offset::vector<RamState> ram;
};

struct ProcessorTest
{
    CpuState initial;
    CpuState final;
    cista::offset::vector<BusState> cycles;
};

struct ProcessorTestBus
{
    std::array<std::uint8_t, addr_space_size> ram{};
    cista::offset::vector<BusState> cycles{};

    std::uint8_t read_byte(std::uint16_t addr)
    {
        const auto data = ram.at(addr);
        cycles.emplace_back(addr, data, "read");
        return data;
    }

    void write_byte(std::uint16_t addr, std::uint8_t data)
    {
        cycles.emplace_back(addr, data, "write");
        ram.at(addr) = data;
    }
};

void run(const char *path)
{
    const auto buf = cista::mmap(path, cista::mmap::protection::READ);
    const auto *tests = cista::deserialize<cista::offset::vector<ProcessorTest>>(buf);

    ProcessorTestBus bus{};
    zcnes::Cpu cpu{&bus};

    for (const auto &test : *tests)
    {
        cpu.pc = test.initial.pc;
        cpu.s = test.initial.s;
        cpu.a = test.initial.a;
        cpu.x = test.initial.x;
        cpu.y = test.initial.y;
        cpu.p = std::bit_cast<zcnes::Cpu<ProcessorTestBus>::Status>(test.initial.p);
        for (const auto &[addr, data] : test.initial.ram)
        {
            bus.ram.at(addr) = data;
        }
        bus.cycles.clear();

        cpu.step();

        zcnes_assert(cpu.pc == test.final.pc);
        zcnes_assert(cpu.s == test.final.s);
        zcnes_assert(cpu.a == test.final.a);
        zcnes_assert(cpu.x == test.final.x);
        zcnes_assert(cpu.y == test.final.y);
        zcnes_assert(std::bit_cast<std::uint8_t>(cpu.p) == test.final.p);
        for (const auto &[addr, data] : test.final.ram)
        {
            zcnes_assert(bus.ram.at(addr) == data);
        }
        zcnes_assert(bus.cycles == test.cycles);
    }
}

int main(int argc, char *argv[])
{
    std::span<char *> args{argv, static_cast<std::size_t>(argc)};
    if (args.size() < 2)
    {
        std::cerr << "error: no input file\n";
        return EXIT_FAILURE;
    }

    try
    {
        const auto *path = args[1];
        run(path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
