#include "bus.hpp"

#include <cstdint>

namespace zcnes {

std::uint8_t CpuBus::read(std::uint16_t addr) {
    if (addr <= 0x1FFF) {
        // 0x0800-0x1FFF are mirrors of 0x0000-0x07FF.
        return ram[addr & 0x07FF];
    }

    return 0;
}

void CpuBus::write(std::uint16_t addr, std::uint8_t data) {
    if (addr <= 0x1FFF) {
        // 0x0800-0x1FFF are mirrors of 0x0000-0x07FF.
        ram[addr & 0x07FF] = data;
    }
}

}
