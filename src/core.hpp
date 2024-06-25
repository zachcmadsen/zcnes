#pragma once

#include <zcnes/core.hpp>

#include "bus.hpp"
#include "cpu.hpp"

namespace zcnes
{

class Core final : public CoreBase
{
  public:
    Core();

    void step() override;

  private:
    Bus bus{};
    Cpu<Bus> cpu;
};

} // namespace zcnes
