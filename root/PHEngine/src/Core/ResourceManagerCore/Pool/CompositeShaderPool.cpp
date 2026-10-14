#include "CompositeShaderPool.h"

namespace Resources {
std::string CompositeShaderPool::ToString() const
{
    return "CompositeShaderPool";
}

CompositeShaderPool* CompositeShaderPool::GetInstance()
{
    static CompositeShaderPool instance;
    return &instance;
}

#if DEBUG

void CompositeShaderPool::RecompileShaders()
{
    for (const auto& shader_pair : resourceMap) {
        auto shader = shader_pair.second;
        if (shader) {
            shader->RecompileShader();
        }
    }
}

#endif
} // namespace Resources
