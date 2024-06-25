#pragma once

#include <format>
#include <source_location>
#include <stdexcept>

constexpr void zcnes_assert(bool condition, const std::source_location location = std::source_location::current())
{
    if (!condition)
    {
        throw std::runtime_error(
            std::format("assertion failed at {}:{}:{}", location.file_name(), location.line(), location.column()));
    }
}
