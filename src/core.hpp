#pragma once

#include <cstdint>
#include <optional>
#include <span>

#include <zcnes/core.hpp>

#include "bus.hpp"
#include "cart.hpp"
#include "cpu.hpp"
#include "ppu.hpp"

namespace zcnes
{

class Core final : public CoreBase
{
  public:
    explicit Core(std::span<const std::uint8_t> rom);

    void step() override;

    [[nodiscard]] std::optional<std::uint8_t> peek(std::uint16_t addr) const override;

  private:
    Cart cart;
    Bus bus;
    Cpu<Bus> cpu;
    Ppu ppu;
};

} // namespace zcnes
