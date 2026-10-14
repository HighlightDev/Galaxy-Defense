#include "SSBOPool.h"

namespace Resources {

std::unique_ptr<SSBOPool> SSBOPool::m_instance = nullptr;

std::string SSBOPool::ToString() const
{
    return "SSBOPool";
}

std::unique_ptr<SSBOPool>& SSBOPool::GetInstance()
{
    if (m_instance == nullptr) {
        m_instance = std::make_unique<SSBOPool>();
    }
    return m_instance;
}

void SSBOPool::ReloadInstance()
{
    m_instance = std::make_unique<SSBOPool>();
}

} // namespace Resources