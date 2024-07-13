#pragma once

#include <source_location>
#include <stdexcept>
#include <string>

namespace zcnes
{

constexpr void check(bool condition, const std::source_location location = std::source_location::current())
{
    if (!condition)
    {
        throw std::runtime_error("assertion failed at " + std::string{location.file_name()} + ":" +
                                 std::to_string(location.line()) + ":" + std::to_string(location.column()));
    }
}

} // namespace zcnes
