#include "core.hpp"

namespace zcnes {

Core::Core() : cpu(bus) {
}

void Core::Step() {
    cpu.Step();
}

}
