#include <cstdint>
#include <vector>

#include <fmt/core.h>
#include <rfl/json/load.hpp>

#include "processor.hpp"

#ifdef _ZCNES_TESTS_PATH
#define ZCNES_TESTS_PATH _ZCNES_TESTS_PATH
#else
#define ZCNES_TESTS_PATH ""
#endif

std::vector<ProcessorTest> LoadTests(std::uint8_t opcode) {
    const auto filename =
        fmt::format("{}/ProcessorTests/{:02x}.json", ZCNES_TESTS_PATH, opcode);
    return rfl::json::load<std::vector<ProcessorTest>>(filename).value();
}
