#include <concepts>
#include <cstdint>

namespace zcnes
{

/// A type that can be addressed to read and write bytes.
template <typename T>
concept Addressable = requires(T t, std::uint16_t addr, std::uint8_t data) {
    { t.read_byte(addr) } -> std::same_as<std::uint8_t>;
    { t.write_byte(addr, data) } -> std::same_as<void>;
};

} // namespace zcnes
