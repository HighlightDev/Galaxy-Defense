#include "FreeTypeFontPool.h"

namespace Resources {
std::unique_ptr<FreeTypeFontPool> FreeTypeFontPool::m_instance;

std::string FreeTypeFontPool::ToString() const
{
    return "FreeTypeFontPool";
}

std::unique_ptr<FreeTypeFontPool>& FreeTypeFontPool::GetInstance()
{
    if (!m_instance)
        m_instance = std::make_unique<FreeTypeFontPool>();

    return m_instance;
}

void FreeTypeFontPool::ReloadInstance()
{
    if (m_instance)
        m_instance.reset();
}
} // namespace Resources
