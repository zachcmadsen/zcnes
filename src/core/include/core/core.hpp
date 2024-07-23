#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <span>

namespace zcnes
{

class CoreBase
{
  public:
    virtual ~CoreBase() = default;

    virtual void step() = 0;
    virtual void fill(std::span<std::int16_t> samples) = 0;
    [[nodiscard]] virtual std::optional<std::uint8_t> peek(std::uint16_t addr) const = 0;
};

std::unique_ptr<CoreBase> make_core(std::span<const std::uint8_t> rom);

} // namespace zcnes
