#pragma once

#include <cpu.h>

#include "bus.h"

namespace zcnes {

class Core {
  public:
    Core();

    void Step();

  private:
    Bus bus;
    Cpu<Bus> cpu;
};

}
