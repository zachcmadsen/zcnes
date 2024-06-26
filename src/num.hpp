#pragma once

#include <cstdint>

#if defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow) && __has_builtin(__builtin_sub_overflow) && __has_builtin(__builtin_addcb)
#define ZCNES_HAS_BUILTINS 1
#endif
#endif

namespace zcnes
{

/// Combines `high` and `low` into a 16-bit integer.
///
/// The implementation is copied from https://stackoverflow.com/a/57320436.
inline constexpr std::uint16_t combine(std::uint8_t high, std::uint8_t low)
{
    return static_cast<std::uint16_t>(static_cast<unsigned>(high) << 8 | static_cast<unsigned>(low));
}

/// Computes `lhs` + `rhs` and returns a boolean indicating whether overflow
/// occurred.
inline constexpr bool overflowing_add(std::uint8_t lhs, std::uint8_t rhs, std::uint8_t *res)
{
#ifdef ZCNES_HAS_BUILTINS
    return __builtin_add_overflow(lhs, rhs, res);
#else
    *res = lhs + rhs;
    return *res < lhs;
#endif
}

/// Computes `lhs` - `rhs` and returns a boolean indicating whether overflow
/// occurred.
inline constexpr bool overflowing_sub(std::uint8_t lhs, std::uint8_t rhs, std::uint8_t *res)
{
#ifdef ZCNES_HAS_BUILTINS
    return __builtin_sub_overflow(lhs, rhs, res);
#else
    *res = lhs - rhs;
    return *res > lhs;
#endif
}

/// Computes `lhs` + `rhs` + `carry`, returns the sum, and stores the ouput
/// carry in `out`.
inline std::uint8_t carrying_add(std::uint8_t lhs, std::uint8_t rhs, std::uint8_t carry, std::uint8_t *out)
{
#ifdef ZCNES_HAS_BUILTINS
    return __builtin_addcb(lhs, rhs, carry, out);
#else
    std::uint8_t res;
    std::uint8_t overflow = overflowing_add(lhs, rhs, &res);
    overflow |= overflowing_add(res, carry, &res);
    *out = overflow;
    return res;
#endif
}

} // namespace zcnes
