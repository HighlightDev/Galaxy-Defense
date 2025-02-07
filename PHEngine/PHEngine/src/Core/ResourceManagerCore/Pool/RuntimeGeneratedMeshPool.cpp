#include "RuntimeGeneratedMeshPool.h"

namespace Resources {
std::unique_ptr<RuntimeGeneratedMeshPool> RuntimeGeneratedMeshPool::m_instance;

std::string RuntimeGeneratedMeshPool::ToString() const
{
    return "RuntimeGeneratedMeshPool";
}

std::unique_ptr<RuntimeGeneratedMeshPool>& RuntimeGeneratedMeshPool::GetInstance()
{
    if (!m_instance)
        m_instance = std::make_unique<RuntimeGeneratedMeshPool>();

    return m_instance;
}

void RuntimeGeneratedMeshPool::ReloadInstance()
{
    if (m_instance)
        m_instance.reset();
}
} // namespace Resources
