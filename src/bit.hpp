#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>

namespace zcnes
{

/// Returns true if and only if bit `n` of `byte` is set.
template <std::size_t n> inline constexpr bool bit(std::uint8_t byte)
{
    static_assert(n < (sizeof(std::uint8_t) * CHAR_BIT));
    return byte & (1 << n);
}

/// Returns true if and only if the least significant bit of `byte` is set.
inline constexpr bool lsb(std::uint8_t byte)
{
    return bit<0>(byte);
}

/// Returns true if and only if the most significant bit of `byte` is set.
inline constexpr bool msb(std::uint8_t byte)
{
    return bit<7>(byte);
}

} // namespace zcnes
