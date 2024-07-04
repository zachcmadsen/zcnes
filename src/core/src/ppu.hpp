#pragma once

#include <cpu/cpu.hpp>

#include "bus.hpp"
#include <cstdint>

namespace zcnes
{

class Ppu
{
  public:
    explicit Ppu(Cpu<Bus> *cpu);

    void run(std::uint64_t cycles);

    std::uint8_t read(std::uint16_t addr);

    void write(std::uint16_t addr, std::uint8_t data);

  private:
    struct Ctrl
    {
        std::uint8_t base_nt_addr : 2;
        bool vram_addr_inc : 1;
        bool sprite_pt_addr : 1;
        bool bg_pt_addr : 1;
        bool sprite_size : 1;
        bool master_slave : 1;
        bool nmi : 1;
    };
    static_assert(sizeof(Ctrl) == 1);

    struct Status
    {
        std::uint8_t : 5;
        bool sprite_overflow : 1;
        bool sprite_0_hit : 1;
        bool vblank : 1;
    };
    static_assert(sizeof(Status) == 1);

    struct Mask
    {
        bool greyscale : 1;
        bool show_left_bg : 1;
        bool show_left_sprites : 1;
        bool show_bg : 1;
        bool show_sprites : 1;
        bool emphasize_red : 1;
        bool emphasize_green : 1;
        bool emphasize_blue : 1;
    };
    static_assert(sizeof(Mask) == 1);

    Ctrl ctrl{};
    Mask mask{};
    Status status{.sprite_overflow = true, .vblank = true};

    int scanline{};
    int cycle{};

    bool suppress_nmi{false};

    std::uint64_t master_clock{};

    std::uint8_t cpu_bus{};

    std::uint64_t prev_ticks{};

    bool rendering_enabled{false};

    bool is_odd_frame{false};

    Cpu<Bus> *cpu;

    void tick();
};

} // namespace zcnes
