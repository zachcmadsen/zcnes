#include <memory>

#include <zcnes/core.hpp>

#include "core.hpp"

namespace zcnes {

Core::Core() : cpu(bus) {
}

void Core::step() {
}

std::unique_ptr<CoreBase> make_core() {
    return std::make_unique<Core>();
}

}
