#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

#define ZCNES_PROCESSOR_TESTS
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

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,readability-function-cognitive-complexity)
void run(std::uint8_t opcode)
{
    ProcessorTestBus bus{};
    zcnes::Cpu cpu{&bus};

    const auto tests = load_tests(opcode);
    for (const auto &test : tests)
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

        REQUIRE(cpu.pc == test.final.pc);
        REQUIRE(cpu.s == test.final.s);
        REQUIRE(cpu.a == test.final.a);
        REQUIRE(cpu.x == test.final.x);
        REQUIRE(cpu.y == test.final.y);
        REQUIRE(std::bit_cast<std::uint8_t>(cpu.p) == test.final.p);
        for (const auto &[addr, data] : test.final.ram)
        {
            REQUIRE(bus.ram.at(addr) == data);
        }
        REQUIRE(bus.cycles == test.cycles);
    }
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,readability-function-cognitive-complexity)

// clang-format off
TEST_CASE("ProcessorTests.00") { run(0x00); }
TEST_CASE("ProcessorTests.01") { run(0x01); }
// TEST_CASE("ProcessorTests.02") { run(0x02); }
TEST_CASE("ProcessorTests.03") { run(0x03); }
TEST_CASE("ProcessorTests.04") { run(0x04); }
TEST_CASE("ProcessorTests.05") { run(0x05); }
TEST_CASE("ProcessorTests.06") { run(0x06); }
TEST_CASE("ProcessorTests.07") { run(0x07); }
TEST_CASE("ProcessorTests.08") { run(0x08); }
TEST_CASE("ProcessorTests.09") { run(0x09); }
TEST_CASE("ProcessorTests.0A") { run(0x0A); }
TEST_CASE("ProcessorTests.0B") { run(0x0B); }
TEST_CASE("ProcessorTests.0C") { run(0x0C); }
TEST_CASE("ProcessorTests.0D") { run(0x0D); }
TEST_CASE("ProcessorTests.0E") { run(0x0E); }
TEST_CASE("ProcessorTests.0F") { run(0x0F); }
TEST_CASE("ProcessorTests.10") { run(0x10); }
TEST_CASE("ProcessorTests.11") { run(0x11); }
// TEST_CASE("ProcessorTests.12") { run(0x12); }
TEST_CASE("ProcessorTests.13") { run(0x13); }
TEST_CASE("ProcessorTests.14") { run(0x14); }
TEST_CASE("ProcessorTests.15") { run(0x15); }
TEST_CASE("ProcessorTests.16") { run(0x16); }
TEST_CASE("ProcessorTests.17") { run(0x17); }
TEST_CASE("ProcessorTests.18") { run(0x18); }
TEST_CASE("ProcessorTests.19") { run(0x19); }
TEST_CASE("ProcessorTests.1A") { run(0x1A); }
TEST_CASE("ProcessorTests.1B") { run(0x1B); }
TEST_CASE("ProcessorTests.1C") { run(0x1C); }
TEST_CASE("ProcessorTests.1D") { run(0x1D); }
TEST_CASE("ProcessorTests.1E") { run(0x1E); }
TEST_CASE("ProcessorTests.1F") { run(0x1F); }
TEST_CASE("ProcessorTests.20") { run(0x20); }
TEST_CASE("ProcessorTests.21") { run(0x21); }
// TEST_CASE("ProcessorTests.22") { run(0x22); }
TEST_CASE("ProcessorTests.23") { run(0x23); }
TEST_CASE("ProcessorTests.24") { run(0x24); }
TEST_CASE("ProcessorTests.25") { run(0x25); }
TEST_CASE("ProcessorTests.26") { run(0x26); }
TEST_CASE("ProcessorTests.27") { run(0x27); }
TEST_CASE("ProcessorTests.28") { run(0x28); }
TEST_CASE("ProcessorTests.29") { run(0x29); }
TEST_CASE("ProcessorTests.2A") { run(0x2A); }
TEST_CASE("ProcessorTests.2B") { run(0x2B); }
TEST_CASE("ProcessorTests.2C") { run(0x2C); }
TEST_CASE("ProcessorTests.2D") { run(0x2D); }
TEST_CASE("ProcessorTests.2E") { run(0x2E); }
TEST_CASE("ProcessorTests.2F") { run(0x2F); }
TEST_CASE("ProcessorTests.30") { run(0x30); }
TEST_CASE("ProcessorTests.31") { run(0x31); }
// TEST_CASE("ProcessorTests.32") { run(0x32); }
TEST_CASE("ProcessorTests.33") { run(0x33); }
TEST_CASE("ProcessorTests.34") { run(0x34); }
TEST_CASE("ProcessorTests.35") { run(0x35); }
TEST_CASE("ProcessorTests.36") { run(0x36); }
TEST_CASE("ProcessorTests.37") { run(0x37); }
TEST_CASE("ProcessorTests.38") { run(0x38); }
TEST_CASE("ProcessorTests.39") { run(0x39); }
TEST_CASE("ProcessorTests.3A") { run(0x3A); }
TEST_CASE("ProcessorTests.3B") { run(0x3B); }
TEST_CASE("ProcessorTests.3C") { run(0x3C); }
TEST_CASE("ProcessorTests.3D") { run(0x3D); }
TEST_CASE("ProcessorTests.3E") { run(0x3E); }
TEST_CASE("ProcessorTests.3F") { run(0x3F); }
TEST_CASE("ProcessorTests.40") { run(0x40); }
TEST_CASE("ProcessorTests.41") { run(0x41); }
// TEST_CASE("ProcessorTests.42") { run(0x42); }
TEST_CASE("ProcessorTests.43") { run(0x43); }
TEST_CASE("ProcessorTests.44") { run(0x44); }
TEST_CASE("ProcessorTests.45") { run(0x45); }
TEST_CASE("ProcessorTests.46") { run(0x46); }
TEST_CASE("ProcessorTests.47") { run(0x47); }
TEST_CASE("ProcessorTests.48") { run(0x48); }
TEST_CASE("ProcessorTests.49") { run(0x49); }
TEST_CASE("ProcessorTests.4A") { run(0x4A); }
TEST_CASE("ProcessorTests.4B") { run(0x4B); }
TEST_CASE("ProcessorTests.4C") { run(0x4C); }
TEST_CASE("ProcessorTests.4D") { run(0x4D); }
TEST_CASE("ProcessorTests.4E") { run(0x4E); }
TEST_CASE("ProcessorTests.4F") { run(0x4F); }
TEST_CASE("ProcessorTests.50") { run(0x50); }
TEST_CASE("ProcessorTests.51") { run(0x51); }
// TEST_CASE("ProcessorTests.52") { run(0x52); }
TEST_CASE("ProcessorTests.53") { run(0x53); }
TEST_CASE("ProcessorTests.54") { run(0x54); }
TEST_CASE("ProcessorTests.55") { run(0x55); }
TEST_CASE("ProcessorTests.56") { run(0x56); }
TEST_CASE("ProcessorTests.57") { run(0x57); }
TEST_CASE("ProcessorTests.58") { run(0x58); }
TEST_CASE("ProcessorTests.59") { run(0x59); }
TEST_CASE("ProcessorTests.5A") { run(0x5A); }
TEST_CASE("ProcessorTests.5B") { run(0x5B); }
TEST_CASE("ProcessorTests.5C") { run(0x5C); }
TEST_CASE("ProcessorTests.5D") { run(0x5D); }
TEST_CASE("ProcessorTests.5E") { run(0x5E); }
TEST_CASE("ProcessorTests.5F") { run(0x5F); }
TEST_CASE("ProcessorTests.60") { run(0x60); }
TEST_CASE("ProcessorTests.61") { run(0x61); }
// TEST_CASE("ProcessorTests.62") { run(0x62); }
TEST_CASE("ProcessorTests.63") { run(0x63); }
TEST_CASE("ProcessorTests.64") { run(0x64); }
TEST_CASE("ProcessorTests.65") { run(0x65); }
TEST_CASE("ProcessorTests.66") { run(0x66); }
TEST_CASE("ProcessorTests.67") { run(0x67); }
TEST_CASE("ProcessorTests.68") { run(0x68); }
TEST_CASE("ProcessorTests.69") { run(0x69); }
TEST_CASE("ProcessorTests.6A") { run(0x6A); }
TEST_CASE("ProcessorTests.6B") { run(0x6B); }
TEST_CASE("ProcessorTests.6C") { run(0x6C); }
TEST_CASE("ProcessorTests.6D") { run(0x6D); }
TEST_CASE("ProcessorTests.6E") { run(0x6E); }
TEST_CASE("ProcessorTests.6F") { run(0x6F); }
TEST_CASE("ProcessorTests.70") { run(0x70); }
TEST_CASE("ProcessorTests.71") { run(0x71); }
// TEST_CASE("ProcessorTests.72") { run(0x72); }
TEST_CASE("ProcessorTests.73") { run(0x73); }
TEST_CASE("ProcessorTests.74") { run(0x74); }
TEST_CASE("ProcessorTests.75") { run(0x75); }
TEST_CASE("ProcessorTests.76") { run(0x76); }
TEST_CASE("ProcessorTests.77") { run(0x77); }
TEST_CASE("ProcessorTests.78") { run(0x78); }
TEST_CASE("ProcessorTests.79") { run(0x79); }
TEST_CASE("ProcessorTests.7A") { run(0x7A); }
TEST_CASE("ProcessorTests.7B") { run(0x7B); }
TEST_CASE("ProcessorTests.7C") { run(0x7C); }
TEST_CASE("ProcessorTests.7D") { run(0x7D); }
TEST_CASE("ProcessorTests.7E") { run(0x7E); }
TEST_CASE("ProcessorTests.7F") { run(0x7F); }
TEST_CASE("ProcessorTests.80") { run(0x80); }
TEST_CASE("ProcessorTests.81") { run(0x81); }
TEST_CASE("ProcessorTests.82") { run(0x82); }
TEST_CASE("ProcessorTests.83") { run(0x83); }
TEST_CASE("ProcessorTests.84") { run(0x84); }
TEST_CASE("ProcessorTests.85") { run(0x85); }
TEST_CASE("ProcessorTests.86") { run(0x86); }
TEST_CASE("ProcessorTests.87") { run(0x87); }
TEST_CASE("ProcessorTests.88") { run(0x88); }
TEST_CASE("ProcessorTests.89") { run(0x89); }
TEST_CASE("ProcessorTests.8A") { run(0x8A); }
// TEST_CASE("ProcessorTests.8B") { run(0x8B); }
TEST_CASE("ProcessorTests.8C") { run(0x8C); }
TEST_CASE("ProcessorTests.8D") { run(0x8D); }
TEST_CASE("ProcessorTests.8E") { run(0x8E); }
TEST_CASE("ProcessorTests.8F") { run(0x8F); }
TEST_CASE("ProcessorTests.90") { run(0x90); }
TEST_CASE("ProcessorTests.91") { run(0x91); }
// TEST_CASE("ProcessorTests.92") { run(0x92); }
TEST_CASE("ProcessorTests.93") { run(0x93); }
TEST_CASE("ProcessorTests.94") { run(0x94); }
TEST_CASE("ProcessorTests.95") { run(0x95); }
TEST_CASE("ProcessorTests.96") { run(0x96); }
TEST_CASE("ProcessorTests.97") { run(0x97); }
TEST_CASE("ProcessorTests.98") { run(0x98); }
TEST_CASE("ProcessorTests.99") { run(0x99); }
TEST_CASE("ProcessorTests.9A") { run(0x9A); }
TEST_CASE("ProcessorTests.9B") { run(0x9B); }
TEST_CASE("ProcessorTests.9C") { run(0x9C); }
TEST_CASE("ProcessorTests.9D") { run(0x9D); }
TEST_CASE("ProcessorTests.9E") { run(0x9E); }
TEST_CASE("ProcessorTests.9F") { run(0x9F); }
TEST_CASE("ProcessorTests.A0") { run(0xA0); }
TEST_CASE("ProcessorTests.A1") { run(0xA1); }
TEST_CASE("ProcessorTests.A2") { run(0xA2); }
TEST_CASE("ProcessorTests.A3") { run(0xA3); }
TEST_CASE("ProcessorTests.A4") { run(0xA4); }
TEST_CASE("ProcessorTests.A5") { run(0xA5); }
TEST_CASE("ProcessorTests.A6") { run(0xA6); }
TEST_CASE("ProcessorTests.A7") { run(0xA7); }
TEST_CASE("ProcessorTests.A8") { run(0xA8); }
TEST_CASE("ProcessorTests.A9") { run(0xA9); }
TEST_CASE("ProcessorTests.AA") { run(0xAA); }
// TEST_CASE("ProcessorTests.AB") { run(0xAB); }
TEST_CASE("ProcessorTests.AC") { run(0xAC); }
TEST_CASE("ProcessorTests.AD") { run(0xAD); }
TEST_CASE("ProcessorTests.AE") { run(0xAE); }
TEST_CASE("ProcessorTests.AF") { run(0xAF); }
TEST_CASE("ProcessorTests.B0") { run(0xB0); }
TEST_CASE("ProcessorTests.B1") { run(0xB1); }
// TEST_CASE("ProcessorTests.B2") { run(0xB2); }
TEST_CASE("ProcessorTests.B3") { run(0xB3); }
TEST_CASE("ProcessorTests.B4") { run(0xB4); }
TEST_CASE("ProcessorTests.B5") { run(0xB5); }
TEST_CASE("ProcessorTests.B6") { run(0xB6); }
TEST_CASE("ProcessorTests.B7") { run(0xB7); }
TEST_CASE("ProcessorTests.B8") { run(0xB8); }
TEST_CASE("ProcessorTests.B9") { run(0xB9); }
TEST_CASE("ProcessorTests.BA") { run(0xBA); }
TEST_CASE("ProcessorTests.BB") { run(0xBB); }
TEST_CASE("ProcessorTests.BC") { run(0xBC); }
TEST_CASE("ProcessorTests.BD") { run(0xBD); }
TEST_CASE("ProcessorTests.BE") { run(0xBE); }
TEST_CASE("ProcessorTests.BF") { run(0xBF); }
TEST_CASE("ProcessorTests.C0") { run(0xC0); }
TEST_CASE("ProcessorTests.C1") { run(0xC1); }
TEST_CASE("ProcessorTests.C2") { run(0xC2); }
TEST_CASE("ProcessorTests.C3") { run(0xC3); }
TEST_CASE("ProcessorTests.C4") { run(0xC4); }
TEST_CASE("ProcessorTests.C5") { run(0xC5); }
TEST_CASE("ProcessorTests.C6") { run(0xC6); }
TEST_CASE("ProcessorTests.C7") { run(0xC7); }
TEST_CASE("ProcessorTests.C8") { run(0xC8); }
TEST_CASE("ProcessorTests.C9") { run(0xC9); }
TEST_CASE("ProcessorTests.CA") { run(0xCA); }
TEST_CASE("ProcessorTests.CB") { run(0xCB); }
TEST_CASE("ProcessorTests.CC") { run(0xCC); }
TEST_CASE("ProcessorTests.CD") { run(0xCD); }
TEST_CASE("ProcessorTests.CE") { run(0xCE); }
TEST_CASE("ProcessorTests.CF") { run(0xCF); }
TEST_CASE("ProcessorTests.D0") { run(0xD0); }
TEST_CASE("ProcessorTests.D1") { run(0xD1); }
// TEST_CASE("ProcessorTests.D2") { run(0xD2); }
TEST_CASE("ProcessorTests.D3") { run(0xD3); }
TEST_CASE("ProcessorTests.D4") { run(0xD4); }
TEST_CASE("ProcessorTests.D5") { run(0xD5); }
TEST_CASE("ProcessorTests.D6") { run(0xD6); }
TEST_CASE("ProcessorTests.D7") { run(0xD7); }
TEST_CASE("ProcessorTests.D8") { run(0xD8); }
TEST_CASE("ProcessorTests.D9") { run(0xD9); }
TEST_CASE("ProcessorTests.DA") { run(0xDA); }
TEST_CASE("ProcessorTests.DB") { run(0xDB); }
TEST_CASE("ProcessorTests.DC") { run(0xDC); }
TEST_CASE("ProcessorTests.DD") { run(0xDD); }
TEST_CASE("ProcessorTests.DE") { run(0xDE); }
TEST_CASE("ProcessorTests.DF") { run(0xDF); }
TEST_CASE("ProcessorTests.E0") { run(0xE0); }
TEST_CASE("ProcessorTests.E1") { run(0xE1); }
TEST_CASE("ProcessorTests.E2") { run(0xE2); }
TEST_CASE("ProcessorTests.E3") { run(0xE3); }
TEST_CASE("ProcessorTests.E4") { run(0xE4); }
TEST_CASE("ProcessorTests.E5") { run(0xE5); }
TEST_CASE("ProcessorTests.E6") { run(0xE6); }
TEST_CASE("ProcessorTests.E7") { run(0xE7); }
TEST_CASE("ProcessorTests.E8") { run(0xE8); }
TEST_CASE("ProcessorTests.E9") { run(0xE9); }
TEST_CASE("ProcessorTests.EA") { run(0xEA); }
TEST_CASE("ProcessorTests.EB") { run(0xEB); }
TEST_CASE("ProcessorTests.EC") { run(0xEC); }
TEST_CASE("ProcessorTests.ED") { run(0xED); }
TEST_CASE("ProcessorTests.EE") { run(0xEE); }
TEST_CASE("ProcessorTests.EF") { run(0xEF); }
TEST_CASE("ProcessorTests.F0") { run(0xF0); }
TEST_CASE("ProcessorTests.F1") { run(0xF1); }
// TEST_CASE("ProcessorTests.F2") { run(0xF2); }
TEST_CASE("ProcessorTests.F3") { run(0xF3); }
TEST_CASE("ProcessorTests.F4") { run(0xF4); }
TEST_CASE("ProcessorTests.F5") { run(0xF5); }
TEST_CASE("ProcessorTests.F6") { run(0xF6); }
TEST_CASE("ProcessorTests.F7") { run(0xF7); }
TEST_CASE("ProcessorTests.F8") { run(0xF8); }
TEST_CASE("ProcessorTests.F9") { run(0xF9); }
TEST_CASE("ProcessorTests.FA") { run(0xFA); }
TEST_CASE("ProcessorTests.FB") { run(0xFB); }
TEST_CASE("ProcessorTests.FC") { run(0xFC); }
TEST_CASE("ProcessorTests.FD") { run(0xFD); }
TEST_CASE("ProcessorTests.FE") { run(0xFE); }
TEST_CASE("ProcessorTests.FF") { run(0xFF); }
// clang-format on

} // namespace
