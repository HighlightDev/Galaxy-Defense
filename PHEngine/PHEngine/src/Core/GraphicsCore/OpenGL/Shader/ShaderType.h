#pragma once
#include "Core/CommonCore/Assertion.h"

#include <gl/glew.h>
#include <stdint.h>

#include <cstddef>

namespace Graphics {
namespace OpenGL {
enum eShaderType : int32_t {
    VertexShader = 0x01,
    FragmentShader = VertexShader << 1,
    GeometryShader = FragmentShader << 1,
    TesselationControlShader = GeometryShader << 1,
    TesselationEvaluationShader = TesselationControlShader << 1,
    ComputeShader = TesselationEvaluationShader << 1
};

int32_t MapShaderTypeToOpenGLConstant(const eShaderType internalShaderType);

std::string ShaderTypeToString(const eShaderType shaderType);

enum eMemoryBarrierType : uint32_t {
    VertexAttribArrayBarrierBit = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
    ElementArrayBarrierBit = GL_ELEMENT_ARRAY_BARRIER_BIT,
    UniformBarrierBit = GL_UNIFORM_BARRIER_BIT,
    TextureFetchBarrierBit = GL_TEXTURE_FETCH_BARRIER_BIT,
    ShaderImageAccessBarrierBit = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
    CommandBarrierBit = GL_COMMAND_BARRIER_BIT,
    PixelBufferBarrierBit = GL_PIXEL_BUFFER_BARRIER_BIT,
    TextureUpdateBarrierBit = GL_TEXTURE_UPDATE_BARRIER_BIT,
    BufferUpdateBarrierBit = GL_BUFFER_UPDATE_BARRIER_BIT,
    FramebufferBarrierBit = GL_FRAMEBUFFER_BARRIER_BIT,
    TransformFeedbackBarrierBit = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
    AtomicCounterBarrierBit = GL_ATOMIC_COUNTER_BARRIER_BIT,
    ShaderStorageBarrierBit = GL_SHADER_STORAGE_BARRIER_BIT,
    AllBarrierBits = GL_ALL_BARRIER_BITS
};
} // namespace OpenGL
} // namespace Graphics