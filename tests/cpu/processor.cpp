#include "processor.h"

#include <cpu.h>
#include <doctest/doctest.h>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t AddrSpaceSize = 0x10000;

struct ProcessorTestBus {
    std::array<std::uint8_t, AddrSpaceSize> ram{};
    std::vector<BusState> cycles{};

    std::uint8_t Read(std::uint16_t addr) {
        const auto data = ram[addr];
        cycles.emplace_back(addr, data, "read");
        return data;
    }

    void Write(std::uint16_t addr, std::uint8_t data) {
        cycles.emplace_back(addr, data, "write");
        ram[addr] = data;
    }
};

void Run(std::uint8_t opcode) {
    ProcessorTestBus bus;
    zcnes::Cpu cpu(bus);

    const auto tests = LoadTests(opcode);
    for (const auto &test : tests) {
        cpu.pc = test.initial.pc;
        cpu.s = test.initial.s;
        cpu.a = test.initial.a;
        cpu.x = test.initial.x;
        cpu.y = test.initial.y;
        cpu.p = std::bit_cast<zcnes::Status>(test.initial.p);
        for (const auto &[addr, data] : test.initial.ram) {
            bus.ram[addr] = data;
        }
        bus.cycles.clear();

        cpu.Step();

        REQUIRE(cpu.pc == test.final.pc);
        REQUIRE(cpu.s == test.final.s);
        REQUIRE(cpu.a == test.final.a);
        REQUIRE(cpu.x == test.final.x);
        REQUIRE(cpu.y == test.final.y);
        REQUIRE(std::bit_cast<std::uint8_t>(cpu.p) == test.final.p);
        for (const auto &[addr, data] : test.final.ram) {
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

}
