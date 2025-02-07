#include "InstancedMeshPool.h"

namespace Resources {
std::unique_ptr<InstancedMeshPool> InstancedMeshPool::m_instance;

std::string InstancedMeshPool::ToString() const
{
    return "InstancedMeshPool";
}

std::unique_ptr<InstancedMeshPool>& InstancedMeshPool::GetInstance()
{
    if (!m_instance)
        m_instance = std::make_unique<InstancedMeshPool>();

    return m_instance;
}

void InstancedMeshPool::ReloadInstance()
{
    if (m_instance)
        m_instance.reset();
}

} // namespace Resources
