#pragma once

#include <memory>

namespace zcnes
{

class CoreBase
{
  public:
    virtual ~CoreBase() = default;

    virtual void step() = 0;
};

std::unique_ptr<CoreBase> make_core();

} // namespace zcnes
