#pragma once

#include <cpu/cpu.hpp>

#include "bus.hpp"

namespace zcnes {

class Emu {
  public:
    Emu();

    void step();

  private:
    CpuBus bus{};
    Cpu<CpuBus> cpu;
};

}
