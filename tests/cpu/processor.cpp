#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <cpu.hpp>
#include <doctest/doctest.h>

#include "json.hpp"

namespace
{

constexpr std::size_t addr_space_size = 0x10000;

struct ProcessorTestBus
{
    std::array<std::uint8_t, addr_space_size> ram{};
    std::vector<BusState> cycles{};

    std::uint8_t read(std::uint16_t addr)
    {
        const auto data = ram[addr];
        cycles.emplace_back(addr, data, "read");
        return data;
    }

    void write(std::uint16_t addr, std::uint8_t data)
    {
        cycles.emplace_back(addr, data, "write");
        ram[addr] = data;
    }
};

void Run(std::uint8_t opcode)
{
    ProcessorTestBus bus;
    zcnes::Cpu cpu(bus);

    const auto tests = load_tests(opcode);
    for (const auto &test : tests)
    {
        cpu.pc = test.initial.pc;
        cpu.s = test.initial.s;
        cpu.a = test.initial.a;
        cpu.x = test.initial.x;
        cpu.y = test.initial.y;
        cpu.p = std::bit_cast<zcnes::Status>(test.initial.p);
        for (const auto &[addr, data] : test.initial.ram)
        {
            bus.ram[addr] = data;
        }
        bus.cycles.clear();

        cpu.step();

        REQUIRE(cpu.pc == test.final.pc);
        REQUIRE(cpu.s == test.final.s);
        REQUIRE(cpu.a == test.final.a);
        REQUIRE(cpu.x == test.final.x);
        REQUIRE(cpu.y == test.final.y);
        REQUIRE(std::bit_cast<std::uint8_t>(cpu.p) == test.final.p);
        for (const auto &[addr, data] : test.final.ram)
        {
            REQUIRE(bus.ram[addr] == data);
        }
        REQUIRE(bus.cycles == test.cycles);
    }
}

// clang-format off
TEST_CASE("ProcessorTests.A5") { Run(0xA5); }
TEST_CASE("ProcessorTests.AD") { Run(0xAD); }
TEST_CASE("ProcessorTests.B5") { Run(0xB5); }
TEST_CASE("ProcessorTests.B6") { Run(0xB6); }
TEST_CASE("ProcessorTests.B9") { Run(0xB9); }
TEST_CASE("ProcessorTests.BD") { Run(0xBD); }
// clang-format on

} // namespace
