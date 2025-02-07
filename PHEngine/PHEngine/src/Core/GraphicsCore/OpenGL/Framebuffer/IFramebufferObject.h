#pragma once

namespace Graphics {
class IFramebufferObject {
public:
    virtual void BindFramebufferAsReadTarget() = 0;

    virtual void BindFramebufferAsDrawTarget() = 0;

    virtual void BindFramebufferAsReadDrawTarget() = 0;
};

} // namespace Graphics
