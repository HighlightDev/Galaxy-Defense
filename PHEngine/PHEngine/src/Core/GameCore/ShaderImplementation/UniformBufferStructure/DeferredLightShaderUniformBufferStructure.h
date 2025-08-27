#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace EngineCore::ShaderImpl::UniformBufferStructure {
template<int32_t MaxDirLightCount>
struct DeferredLightShaderUniformBufferStructure {

    // Directional lights
    glm::vec4 AmbientColors[MaxDirLightCount] = {};
    glm::vec4 DiffuseColors[MaxDirLightCount] = {};
    glm::vec4 SpecularColors[MaxDirLightCount] = {};
    glm::vec4 Directions[MaxDirLightCount] = {};
    glm::mat4 DirectionalLightShadowMatrices[MaxDirLightCount] = {};
    glm::vec4 DirectionalLightAtlasOffset[MaxDirLightCount] = {};
    int32_t DirectionalLightCount{0};

    uint32_t GetSizeInBytes() const
    {
        return sizeof(DeferredLightShaderUniformBufferStructure);
    }

    void* GetRawData() const
    {
        return reinterpret_cast<void*>(const_cast<DeferredLightShaderUniformBufferStructure*>(this));
    }
};
} // namespace EngineCore::ShaderImpl::UniformBufferStructure
