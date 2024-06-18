#pragma once

#include <memory>

namespace zcnes {

class CoreBase {
  public:
    virtual void step() = 0;

    virtual ~CoreBase() = default;
};

std::unique_ptr<CoreBase> make_core();

}
