#include <cstddef>
#include <cstdint>
#include <limits>

namespace zcnes::bit
{

/// Returns true if and only if bit `n` of `byte` is set.
template <std::size_t n> inline bool bit(std::uint8_t byte)
{
    static_assert(n < std::numeric_limits<std::uint8_t>::digits);
    return byte & (1 << n);
}

/// Returns true if and only if the least significant bit of `byte` is set.
inline bool lsb(std::uint8_t byte)
{
    return bit<0>(byte);
}

/// Returns true if and only if the most significant bit of `byte` is set.
inline bool msb(std::uint8_t byte)
{
    return bit<7>(byte);
}

/// Combines `high` and `low` into a 16-bit integer.
///
/// The implementation is copied from https://stackoverflow.com/a/57320436.
inline std::uint16_t combine(std::uint8_t high, std::uint8_t low)
{
    return static_cast<std::uint16_t>(static_cast<unsigned>(high) << 8 | static_cast<unsigned>(low));
}

} // namespace zcnes::bit
