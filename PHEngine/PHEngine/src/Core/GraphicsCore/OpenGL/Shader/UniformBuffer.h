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
#include <vector>

namespace Graphics::OpenGL {

class UniformBufferControlBlock;

class UniformBuffer {

    std::shared_ptr<UniformBufferControlBlock> mControlBlock;

    UniformBufferUserInfo mUserInfo;

    UniformBuffer(const std::shared_ptr<UniformBufferControlBlock>& controlBlock, const UniformBufferUserInfo& userInfo);

    void SetDataExplicit(const void* data, const size_t size);

    void SetDataExplicit(const void* data, const size_t offset, const size_t size);

public:
    UniformBuffer() = default;

    void ResetBuffer();

    static UniformBuffer CreateUniformBuffer(
        const std::shared_ptr<UniformBufferControlBlock>& controlBlock,
        const GLuint shaderProgram,
        const GLsizeiptr size,
        bool dynamic);

    template<typename T>
    typename std::enable_if<std::is_trivial<T>::value, void>::type SetData(const T& data)
    {
        SetDataExplicit(&data, sizeof(T));
    }

    template<typename T>
    typename std::enable_if<std::is_trivial<T>::value, void>::type SetData(const std::vector<T>& dataVector)
    {
        SetDataExplicit(dataVector.data(), dataVector.size() * sizeof(T));
    }

    template<typename T>
    typename std::enable_if<std::is_trivial<T>::value, void>::type SetData(const T& data, const size_t offset)
    {
        SetDataExplicit(&data, offset, sizeof(T));
    }

    uint32_t GetUniformBufferUserId() const;
};
} // namespace Graphics::OpenGL
