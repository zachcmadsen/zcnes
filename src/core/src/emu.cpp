#include <core/emu.hpp>

namespace zcnes {

Emu::Emu() : cpu(bus) {
}

void Emu::step() {
    cpu.step();
}

}
