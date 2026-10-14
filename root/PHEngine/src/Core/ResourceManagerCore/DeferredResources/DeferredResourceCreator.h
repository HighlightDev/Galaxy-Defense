#pragma once

#include "DeferredResource.h"

namespace Resources {

class IDeferredResourceCreator {
public:
    virtual std::shared_ptr<IDeferredResource> GetDeferredResource() = 0;
};
} // namespace Resources