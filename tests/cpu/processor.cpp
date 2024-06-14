#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

#include <cpu/cpu.hpp>
#include <doctest/doctest.h>
#include <fmt/core.h>

#include "util.hpp"

#ifdef _ZCNES_TESTS_PATH
#define ZCNES_TESTS_PATH _ZCNES_TESTS_PATH
#else
#define ZCNES_TESTS_PATH ""
#endif

namespace {

constexpr size_t AddrSpaceSize = 0x10000;

struct ProcessorTestBus {
    std::array<std::uint8_t, AddrSpaceSize> ram{0};

    std::uint8_t read(std::uint16_t addr) {
        return ram[addr];
    }

    void write(std::uint16_t addr, std::uint8_t data) {
        ram[addr] = data;
    }
};

void run(std::string_view opc) {
    ProcessorTestBus bus;
    zcnes::Cpu<ProcessorTestBus> cpu(bus);

    const auto tests = loadTests(
        fmt::format("{}/ProcessorTests/{}.json", ZCNES_TESTS_PATH, opc));
    for (const auto &test : tests) {
        cpu.pc = test.initial.pc;
        cpu.a = test.initial.a;
        for (auto &[addr, data] : test.initial.ram) {
            bus.ram[addr] = static_cast<std::uint8_t>(data);
        }

        cpu.step();

        REQUIRE(cpu.pc == test.final.pc);
        REQUIRE(cpu.a == test.final.a);
        for (auto &[addr, data] : test.final.ram) {
            REQUIRE(bus.ram[addr] == data);
        }
    }
}

}

// clang-format off
TEST_CASE("ProcessorTests.A5") { run("a5"); }
// clang-format on
