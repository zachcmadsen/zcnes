#include <cstdint>

namespace zcnes::num
{

/// Computes `lhs` + `rhs` and returns a boolean indicating whether overflow
/// occurred.
inline bool overflowing_add(std::uint8_t lhs, std::uint8_t rhs, std::uint8_t *res)
{
    *res = lhs + rhs;
    return *res < lhs;
}

/// Computes `lhs` - `rhs` and returns a boolean indicating whether overflow
/// occurred.
inline bool overflowing_sub(std::uint8_t lhs, std::uint8_t rhs, std::uint8_t *res)
{
    *res = lhs - rhs;
    return *res > lhs;
}

/// Computes `lhs` + `rhs` + `carry`, returns the sum, and stores the ouput
/// carry in `out`.
inline std::uint8_t carrying_add(std::uint8_t lhs, std::uint8_t rhs, std::uint8_t carry, std::uint8_t *out)
{
    std::uint8_t res = 0;
    auto overflow = overflowing_add(lhs, rhs, &res);
    overflow |= overflowing_add(res, carry, &res);
    *out = static_cast<std::uint8_t>(overflow);
    return res;
}

} // namespace zcnes::num
