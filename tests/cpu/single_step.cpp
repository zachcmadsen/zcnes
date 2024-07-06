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
#include <common/common.h>
#include <cpu/cpu.hpp>

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
    cista::offset::vector<BusState> cycles;

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

        zcnes::CpuState state{};
        cpu.save_state(state);
        state.pc = test.initial.pc;
        state.s = test.initial.s;
        state.a = test.initial.a;
        state.x = test.initial.x;
        state.y = test.initial.y;
        state.p = test.initial.p;
        cpu.load_state(state);
        for (const auto &[addr, data] : test.initial.ram)
        {
            bus.ram.at(addr) = data;
        }
        bus.cycles.clear();

        cpu.step();

        cpu.save_state(state);
        require(state.pc == test.final.pc);
        require(state.s == test.final.s);
        require(state.a == test.final.a);
        require(state.x == test.final.x);
        require(state.y == test.final.y);
        require(std::bit_cast<std::uint8_t>(state.p) == test.final.p);
        for (const auto &[addr, data] : test.final.ram)
        {
            require(bus.ram.at(addr) == data);
        }
        require(bus.cycles == test.cycles);
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
