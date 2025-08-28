#pragma once

/*
 * DeferredLightShaderUniformBufferStructure
 *
 * This structure defines the layout of uniform buffer data for deferred lighting shaders,
 * supporting a configurable number of directional and point lights. It contains arrays for
 * light properties such as color, direction, shadow matrices, and attenuation, as well as
 * counters for the number of active lights and shadow maps.
 *
 * IMPORTANT: Pay attention to Uniform block layout!
 * The memory layout of this structure must match the layout expected by the shader in GLSL/HLSL.
 * Uniform buffers are transferred directly to the GPU, and any mismatch in alignment, padding,
 * or ordering between CPU-side and GPU-side definitions can lead to incorrect rendering results,
 * undefined behavior, or hard-to-debug graphical artifacts. Always ensure that types, array sizes,
 * and ordering are consistent with the shader code, and consider using std140 or std430 layout rules
 * when designing uniform buffer structures.
 */

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace EngineCore::ShaderImpl::UniformBufferStructure {
#pragma pack(push, 1) // exact fit - no padding
template<int32_t MaxDirLightCount, int32_t MaxPointLightCount>
struct DeferredLightShaderUniformBufferStructure {

    static constexpr int32_t s_maxDirLightCount = MaxDirLightCount;
    static constexpr int32_t s_maxPointLightCount = MaxPointLightCount;

    int32_t DirectionalLightCount{0};
    int32_t DirectionalLightShadowMapCount{0};
    int32_t PointLightCount{0};

    glm::vec4 AmbientColors[MaxDirLightCount] = {};
    glm::vec4 DiffuseColors[MaxDirLightCount] = {};
    glm::vec4 SpecularColors[MaxDirLightCount] = {};
    glm::vec4 Directions[MaxDirLightCount] = {};
    glm::vec4 DirectionalLightAtlasOffset[MaxDirLightCount] = {};

    glm::vec4 PointLightDiffuseColor[MaxPointLightCount] = {};
    glm::vec4 PointLightSpecularColor[MaxPointLightCount] = {};
    glm::vec4 PointLightAttenuation[MaxPointLightCount] = {};
    glm::vec4 PointLightPositionWorld[MaxPointLightCount] = {};

    glm::mat4 DirectionalLightShadowMatrices[MaxDirLightCount] = {};

    uint32_t GetSizeInBytes() const
    {
        return sizeof(DeferredLightShaderUniformBufferStructure);
    }

    void* GetRawData() const
    {
        return reinterpret_cast<void*>(const_cast<DeferredLightShaderUniformBufferStructure*>(this));
    }
};
#pragma pack(pop)
} // namespace EngineCore::ShaderImpl::UniformBufferStructure
