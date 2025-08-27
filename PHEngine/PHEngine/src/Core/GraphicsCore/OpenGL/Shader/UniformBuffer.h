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

    void SetDataExplicit(const void* data, const size_t size, const size_t offset);

public:
    UniformBuffer() = default;

    void ResetBuffer();

    void BindUniformBuffer();

    void BindUniformBufferRange(const uint32_t offset, const uint32_t size);

    static UniformBuffer CreateUniformBuffer(
        const std::shared_ptr<UniformBufferControlBlock>& controlBlock,
        const GLuint shaderProgram,
        const GLsizeiptr size,
        bool dynamic);

    void SetData(const void* data, const size_t size)
    {
        SetDataExplicit(data, size);
    }

    void SetData(const void* data, const size_t size, const size_t offset)
    {
        SetDataExplicit(data, size, offset);
    }

    template<typename T>
    typename std::enable_if_t<std::is_trivial_v<T> && !std::is_pointer_v<T>, void> SetData(const T& data)
    {
        SetDataExplicit(&data, sizeof(T));
    }

    template<typename T>
    typename std::enable_if<std::is_trivial<T>::value, void>::type SetData(const std::vector<T>& dataVector)
    {
        SetDataExplicit(dataVector.data(), dataVector.size() * sizeof(T));
    }

    template<typename T>
    typename std::enable_if_t<std::is_trivial_v<T> && !std::is_pointer_v<T>, void> SetData(const T& data, const size_t offset)
    {
        SetDataExplicit(&data, sizeof(T), offset);
    }

    template<typename T>
    typename std::enable_if<std::is_trivial<T>::value, void>::type SetData(const std::vector<T>& dataVector, const size_t offset)
    {
        SetDataExplicit(dataVector.data(), dataVector.size() * sizeof(T), offset);
    }

    template<typename T>
    typename std::enable_if<std::is_trivial<T>::value, void>::type
    SetData(const std::vector<T>& dataVector, const size_t size, const size_t offset)
    {
        SetDataExplicit(dataVector.data(), size, offset);
    }

    uint32_t GetUniformBufferUserId() const;
};
} // namespace Graphics::OpenGL
