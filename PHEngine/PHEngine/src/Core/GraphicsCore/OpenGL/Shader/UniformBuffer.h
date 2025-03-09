#pragma once

#include "Core/GraphicsCore/OpenGL/UniformBuffer/UniformBufferUserInfo.h"

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <stdint.h>

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

namespace Graphics::OpenGL {

class UniformBufferControlBlock;

class UniformBuffer {

    std::shared_ptr<UniformBufferControlBlock> mControlBlock;

    UniformBufferUserInfo mUserInfo;

    UniformBuffer(const std::shared_ptr<UniformBufferControlBlock>& controlBlock, const UniformBufferUserInfo& userInfo);

public:
    UniformBuffer() = default;

    void SetData(const void* data, const size_t size);

    void ResetBuffer();

    static UniformBuffer CreateUniformBuffer(
        const std::shared_ptr<UniformBufferControlBlock>& controlBlock,
        const GLuint shaderProgram,
        const GLsizeiptr size,
        bool dynamic);

    template<typename T>
    typename std::enable_if<std::is_trivial<T>::value, void>::type SetData(const T& data)
    {
        SetData(&data, sizeof(T));
    }

    uint32_t GetUniformBufferUserId() const;
};
} // namespace Graphics::OpenGL
