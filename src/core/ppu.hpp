#pragma once

#include <cstdint>
#include <functional>
#include <span>

#include "bus.hpp"
#include "cpu.hpp"

namespace zcnes
{

class Ppu
{
  public:
    explicit Ppu(Cpu<Bus> *cpu);

    void run(std::uint64_t cpu_master_clock);

    std::uint8_t read(std::uint16_t addr);

    void write(std::uint16_t addr, std::uint8_t data);

    std::function<void(std::span<const std::uint8_t>)> on_frame;

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

    struct Address
    {
        std::uint16_t coarse_x_scroll : 5;
        std::uint16_t coarse_y_scroll : 5;
        std::uint16_t nt : 2;
        std::uint16_t fine_y_scroll : 3;
    };
    static_assert(sizeof(Address) == 2);

    Ctrl ctrl{};
    Mask mask{};
    Status status{.sprite_overflow = true, .sprite_0_hit = false, .vblank = true};

    Address v{};
    Address t{};
    std::uint8_t x{};
    bool w{};

    std::uint8_t open_bus{};

    int scanline{};
    int cycle{};

    std::uint64_t master_clock{};

    bool is_odd_frame{false};
    bool rendering_enabled{false};
    bool suppress_nmi{false};

    std::uint8_t read_buffer{};

    Cpu<Bus> *cpu;

    std::array<std::uint8_t, 4 * 256 * 240> pixels;

    void tick();

    void increment_v();
};

} // namespace zcnes
