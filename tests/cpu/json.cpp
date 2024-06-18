#include <string_view>
#include <vector>

#include <fmt/core.h>
#include <rfl/json/load.hpp>

#include "processor.hpp"

#ifdef _ZCNES_TESTS_PATH
#define ZCNES_TESTS_PATH _ZCNES_TESTS_PATH
#else
#define ZCNES_TESTS_PATH ""
#endif

std::vector<ProcessorTest> load_tests(std::string_view opc) {
    const auto filename =
        fmt::format("{}/ProcessorTests/{}.json", ZCNES_TESTS_PATH, opc);
    return rfl::json::load<std::vector<ProcessorTest>>(filename).value();
}
