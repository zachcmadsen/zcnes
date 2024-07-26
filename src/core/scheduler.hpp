#pragma once

#include <cstdint>

namespace zcnes
{

class Scheduler
{
  public:
    void tick()
    {
        timestamp += 1;
    }

    [[nodiscard]] std::uint64_t ticks() const
    {
        return timestamp;
    }

  private:
    std::uint64_t timestamp{};
};

} // namespace zcnes
