#pragma once

#include "scheduler.hpp"
#include <cstddef>
#include <cstdint>
#include <span>

#define BLARGG_COMPILER_HAS_NAMESPACE 1
#include <Blip_Buffer.h>
#include <Nes_Apu.h>

namespace zcnes
{

class Apu
{
  public:
    Apu(Scheduler *scheduler);

    void start_frame();

    std::uint8_t read_status();

    void write_register(std::uint16_t addr, std::uint8_t data);

    [[nodiscard]] std::uint64_t cycles_needed(std::size_t num_samples) const;

    std::size_t read_samples(std::span<std::int16_t> samples);

  private:
    Blip_Buffer buf;
    Nes_Apu apu;

    std::uint64_t frame_start_tick;

    Scheduler *scheduler;
};

} // namespace zcnes
