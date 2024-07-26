#include "core.hpp"

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <span>

namespace zcnes
{

Core::Core(std::span<const std::uint8_t> rom)
    : cart{rom}, apu{&scheduler}, bus{&ppu, &cart, &apu, &scheduler}, cpu{&bus}, ppu{&cpu}
{
    cpu.reset();
}

void Core::step()
{
    cpu.step();
}

void Core::fill(std::span<std::int16_t> samples)
{
    apu.start_frame();

    auto x = apu.cycles_needed(samples.size());
    // std::cout << "cycles needed for samples: " << x << " " << samples.size() << '\n';

    const auto samples_ready_tick = scheduler.ticks() + x;

    while (scheduler.ticks() < samples_ready_tick)
    {
        cpu.step();
    }

    const auto read = apu.read_samples(samples);
    if (read != samples.size())
    {
        std::cerr << "there wasn't enough samples for reading\n";
    }
}

std::optional<std::uint8_t> Core::peek(std::uint16_t addr) const
{
    return bus.peek_byte(addr);
}

} // namespace zcnes
