#include "util.hpp"

#include <string>
#include <string_view>
#include <vector>

#include <rfl.hpp>
#include <rfl/json.hpp>
#include <rfl/json/load.hpp>

std::vector<ProcessorTest> loadTests(std::string_view filename) {
    return rfl::json::load<std::vector<ProcessorTest>>(std::string(filename))
        .value();
}
