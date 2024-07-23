#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include "apu.hpp"
#include "cart.hpp"
#include "scheduler.hpp"

namespace zcnes
{

class Ppu;

class Bus
{
  public:
    explicit Bus(Ppu *ppu, Cart *cart, Apu *apu, Scheduler *scheduler);

    std::uint8_t read_byte(std::uint16_t addr);

    void write_byte(std::uint16_t addr, std::uint8_t data);

    [[nodiscard]] std::optional<std::uint8_t> peek_byte(std::uint16_t addr) const;

  private:
    static constexpr std::uint16_t ram_size = 0x800;

    std::array<std::uint8_t, ram_size> ram{};
    Ppu *ppu;
    Cart *cart;
    Apu *apu;
    Scheduler *scheduler;

    std::uint64_t master_clock{2};

    void sync_ppu();
};

} // namespace zcnes
