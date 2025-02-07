#pragma once
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"

#include <memory>

using namespace Graphics;

namespace Resources {

class RenderTargetStorage {
public:
    RenderTargetStorage();

    ~RenderTargetStorage();

    void AddRenderTarget(std::shared_ptr<FramebufferBundle> frambuffer);
};
} // namespace Resources
