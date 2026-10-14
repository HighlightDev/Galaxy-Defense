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
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace EngineCore::ShaderImpl::UniformBufferStructure {
template<int32_t MaxDirLightCount, int32_t MaxPointLightCount, int32_t MaxSpotLightCount>
struct DeferredLightShaderUniformBufferStructure {

    static constexpr int32_t s_maxDirLightCount = MaxDirLightCount;
    static constexpr int32_t s_maxPointLightCount = MaxPointLightCount;
    static constexpr int32_t s_maxSpotLightCount = MaxSpotLightCount;

    glm::vec4 CameraWorldPosition;

    glm::ivec4 LightsCount; // x: DL, y: PL, z: SL
    glm::ivec4 ShadowMapsCount; // x: DL, y: PL, z: SL

    glm::vec4 AmbientColors[MaxDirLightCount] = {};
    glm::vec4 DiffuseColors[MaxDirLightCount] = {};
    glm::vec4 SpecularColors[MaxDirLightCount] = {};
    glm::vec4 Directions[MaxDirLightCount] = {};
    glm::vec4 DirectionalLightAtlasOffset[MaxDirLightCount] = {};

    glm::vec4 PointLightDiffuseColor[MaxPointLightCount] = {};
    glm::vec4 PointLightSpecularColor[MaxPointLightCount] = {};
    glm::vec4 PointLightAttenuation[MaxPointLightCount] = {};
    glm::vec4 PointLightPositionWorld[MaxPointLightCount] = {};
    glm::vec4 PointLightShadowProjectionFarPlane[MaxPointLightCount] = {};

    glm::vec4 SpotlightAmbientColor[MaxSpotLightCount] = {};
    glm::vec4 SpotlightDiffuseColor[MaxSpotLightCount] = {};
    glm::vec4 SpotlightSpecularColor[MaxSpotLightCount] = {};
    glm::vec4 SpotlightDirection[MaxSpotLightCount] = {};
    glm::vec4 SpotlightPosition[MaxSpotLightCount] = {};
    glm::vec4 SpotlightCutoff[MaxSpotLightCount] = {};
    glm::vec4 SpotlightShadowAtlasOffset[MaxSpotLightCount] = {};
    glm::vec4 SpotLightShadowProjectionFarPlane[MaxSpotLightCount] = {};

    glm::mat4 DirectionalLightShadowMatrices[MaxDirLightCount] = {};
    glm::mat4 SpotlightShadowMatrices[MaxSpotLightCount] = {};

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
