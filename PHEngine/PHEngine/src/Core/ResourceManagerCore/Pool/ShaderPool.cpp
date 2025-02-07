#include "ShaderPool.h"

namespace Resources {
std::unique_ptr<ShaderPool> ShaderPool::m_instance;

std::string ShaderPool::ToString() const
{
    return "ShaderPool";
}

std::unique_ptr<ShaderPool>& ShaderPool::GetInstance()
{
    if (!m_instance)
        m_instance = std::make_unique<ShaderPool>();

    return m_instance;
}

void ShaderPool::ReloadInstance()
{
    if (m_instance)
        m_instance.reset();
}
#if DEBUG
void ShaderPool::RecompileShaders()
{
    for (const auto& shader_pair : resourceMap) {
        shader_pair.second->RecompileShader();
    }
}
#endif
} // namespace Resources