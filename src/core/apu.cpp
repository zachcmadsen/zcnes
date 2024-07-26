
#include "apu.hpp"
#include "scheduler.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <stdexcept>

namespace zcnes
{

Apu::Apu(Scheduler *scheduler) : scheduler{scheduler}
{
    const auto *err = buf.set_sample_rate(44100, 1000);
    if (err != nullptr)
    {
        throw std::runtime_error("not enough memory to initilize Blip_Buffer");
    }
    buf.clock_rate(1789773);
    apu.set_output(&buf);
}

void Apu::start_frame()
{
    frame_start_tick = scheduler->ticks();
}

std::uint8_t Apu::read_status()
{
    return static_cast<std::uint8_t>(apu.read_status(scheduler->ticks() - frame_start_tick));
}

void Apu::write_register(std::uint16_t addr, std::uint8_t data)
{
    apu.write_register(scheduler->ticks() - frame_start_tick, addr, data);
}

std::uint64_t Apu::cycles_needed(std::size_t num_samples) const
{
    auto clocks_needed = buf.count_clocks(num_samples);
    // auto factor_clock = buf.clock_rate_factor(clocks_needed);
    // std::cout << "clocks: " << clocks_needed << " factor: " << factor_clock << '\n';
    return static_cast<std::uint64_t>(clocks_needed);
}

std::size_t Apu::read_samples(std::span<std::int16_t> samples)
{
    // std::cout << "end frame with ticks" << (scheduler->ticks() - frame_start_tick) << "\n";

    auto x = scheduler->ticks() - frame_start_tick;
    // std::cout << "end frame with ticks: " << x << "\n";

    apu.end_frame(scheduler->ticks() - frame_start_tick);
    buf.end_frame(scheduler->ticks() - frame_start_tick);
    // frame_start_tick = scheduler->ticks();

    // std::cout << "samples available: " << buf.samples_avail() << "\n";

    const auto read = buf.read_samples(samples.data(), static_cast<int>(samples.size()));

    return static_cast<std::size_t>(read);
}

} // namespace zcnes
