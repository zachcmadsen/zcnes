#include "core.hpp"

#include <memory>

#include <zcnes/core.hpp>

namespace zcnes
{

Core::Core() : cpu{&bus}
{
}

void Core::step()
{
    cpu.step();
}

std::unique_ptr<CoreBase> make_core()
{
    return std::make_unique<Core>();
}

} // namespace zcnes
