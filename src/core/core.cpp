#include "core.hpp"

namespace zcnes
{

Core::Core() : cpu(bus)
{
}

void Core::step()
{
    cpu.step();
}

} // namespace zcnes
