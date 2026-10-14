#include "UniformBufferPool.h"

namespace Resources {
std::unique_ptr<UniformBufferPool> UniformBufferPool::m_instance;

std::string UniformBufferPool::ToString() const
{
    return "UniformBufferPool";
}

std::unique_ptr<UniformBufferPool>& UniformBufferPool::GetInstance()
{
    if (!m_instance)
        m_instance = std::make_unique<UniformBufferPool>();

    return m_instance;
}

void UniformBufferPool::ReloadInstance()
{
    if (m_instance)
        m_instance.reset();
}

std::shared_ptr<UniformBuffer> UniformBufferPool::GetOrAllocateResource(const UniformBufferParameters& arg)
{
    std::shared_ptr<UniformBuffer> uniformBuffer;
    if (m_resourceMap.count(arg.mUserName) == 0) {
        auto controlBlock = std::make_shared<UniformBufferControlBlock>(arg.mBlockName, arg.mBindingPoint, arg.mMemorySize);
        uniformBuffer = std::make_shared<UniformBuffer>(
            UniformBuffer::CreateUniformBuffer(controlBlock, arg.mShaderProgramId, arg.mMemorySize, true));

        std::unordered_map<uint32_t, std::shared_ptr<UniformBuffer>> uniformBuffersMap;
        uniformBuffersMap[uniformBuffer->GetUniformBufferUserId()] = uniformBuffer;
        m_resourceMap.emplace(
            arg.mUserName,
            std::make_shared<std::pair<
                std::shared_ptr<UniformBufferControlBlock>,
                std::unordered_map<uint32_t, std::shared_ptr<UniformBuffer>>>>(controlBlock, uniformBuffersMap));
    } else {
        const auto& controlBlock = m_resourceMap.at(arg.mUserName)->first;
        auto& uniformBuffersMap = m_resourceMap.at(arg.mUserName)->second;
        uniformBuffer = std::make_shared<UniformBuffer>(
            UniformBuffer::CreateUniformBuffer(controlBlock, arg.mShaderProgramId, arg.mMemorySize, true));
        uniformBuffersMap[uniformBuffer->GetUniformBufferUserId()] = uniformBuffer;
    }

    return uniformBuffer;
}

void UniformBufferPool::FreeResource(const std::string& nameOfUser)
{
    if (m_resourceMap.count(nameOfUser) != 0) {
        m_resourceMap.at(nameOfUser)->first->CleanUp();
        m_resourceMap.erase(nameOfUser);
    }
}

void UniformBufferPool::CleanUp()
{
    for (const auto& resource : m_resourceMap) {
        resource.second->first->CleanUp();
    }
    m_resourceMap.clear();
}

} // namespace Resources
