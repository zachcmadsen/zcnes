#include "bus.hpp"

#include <cstdint>

namespace zcnes {

std::uint16_t mirror_ram_addr(std::uint16_t addr) {
    // 0x0800-0x1FFF are mirrors of 0x0000-0x07FF.
    return addr & 0x07FF;
}

std::uint8_t Bus::read(std::uint16_t addr) {
    if (addr <= 0x1FFF) {
        return ram[mirror_ram_addr(addr)];
    }

    return 0;
}

void Bus::write(std::uint16_t addr, std::uint8_t data) {
    if (addr <= 0x1FFF) {
        ram[mirror_ram_addr(addr)] = data;
    }
}
}
