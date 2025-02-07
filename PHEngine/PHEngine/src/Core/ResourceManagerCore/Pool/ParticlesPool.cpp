#include "ParticlesPool.h"

namespace Resources {
std::unique_ptr<ParticlesPool> ParticlesPool::m_instance;

std::string ParticlesPool::ToString() const
{
    return "ParticlesPool";
}

std::unique_ptr<ParticlesPool>& ParticlesPool::GetInstance()
{
    if (!m_instance)
        m_instance = std::make_unique<ParticlesPool>();

    return m_instance;
}

void ParticlesPool::ReloadInstance()
{
    if (m_instance)
        m_instance.reset();
}
} // namespace Resources
