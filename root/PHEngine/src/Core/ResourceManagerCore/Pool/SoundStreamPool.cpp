#include "SoundStreamPool.h"

namespace Resources {
std::unique_ptr<SoundStreamPool> SoundStreamPool::m_instance;

std::string SoundStreamPool::ToString() const
{
    return "SoundStreamPool";
}

std::unique_ptr<SoundStreamPool>& SoundStreamPool::GetInstance()
{
    if (!m_instance)
        m_instance = std::make_unique<SoundStreamPool>();

    return m_instance;
}

void SoundStreamPool::ReloadInstance()
{
    if (m_instance)
        m_instance.reset();
}
} // namespace Resources
