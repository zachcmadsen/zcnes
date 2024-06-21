#include "core.h"

namespace zcnes {

Core::Core() : cpu(bus) {}

void Core::Step() { cpu.Step(); }

}
