#pragma once

#include <cstdint>
#include <optional>
#include <span>

#include "apu.hpp"
#include "bus.hpp"
#include "cart.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "scheduler.hpp"

namespace zcnes
{

class Core
{
  public:
    explicit Core(std::span<const std::uint8_t> rom);

    void step();

    void fill(std::span<std::int16_t> samples);

    [[nodiscard]] std::optional<std::uint8_t> peek(std::uint16_t addr) const;

    template <typename T> void set_on_frame(T &&f)
    {
        ppu.on_frame = f;
    }

  private:
    Scheduler scheduler{};

    Cart cart;
    Apu apu;
    Bus bus;
    Cpu<Bus> cpu;
    Ppu ppu;

    void run(std::uint64_t ticks);
};

} // namespace zcnes
