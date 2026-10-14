#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/UniformBuffer.h"
#include "Core/GraphicsCore/OpenGL/UniformBuffer/UniformBufferControlBlock.h"
#include "PoolParameters/UniformBufferParameters.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

using namespace Graphics::OpenGL;

namespace Resources {

class UniformBufferPool {
    static std::unique_ptr<UniformBufferPool> m_instance;

    std::unordered_map<
        std::string /*name of uniform buffer user*/,
        std::shared_ptr<
            std::pair<std::shared_ptr<UniformBufferControlBlock>, std::unordered_map<uint32_t, std::shared_ptr<UniformBuffer>>>>>
        m_resourceMap;

public:
    UniformBufferPool() = default;

    std::string ToString() const;

    static std::unique_ptr<UniformBufferPool>& GetInstance();

    static void ReloadInstance();

    std::shared_ptr<UniformBuffer> GetOrAllocateResource(const UniformBufferParameters& arg);

    void FreeResource(const std::string& nameOfUser);

    void CleanUp();
};

} // namespace Resources
