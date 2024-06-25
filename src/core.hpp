#pragma once

#include <cstdint>
#include <span>

#include <zcnes/core.hpp>

#include "bus.hpp"
#include "cart.hpp"
#include "cpu.hpp"

namespace zcnes
{

class Core final : public CoreBase
{
  public:
    explicit Core(std::span<const std::uint8_t> rom);

    void step() override;

    [[nodiscard]] std::uint8_t peek(std::uint16_t addr) const override;

  private:
    Cart cart;
    Bus bus;
    Cpu<Bus> cpu;
};

} // namespace zcnes
