#pragma once

#include <cpu.hpp>

#include "bus.hpp"

namespace zcnes
{

class Core
{
  public:
    Core();

    void step();

  private:
    Bus bus;
    Cpu<Bus> cpu;
};

} // namespace zcnes
