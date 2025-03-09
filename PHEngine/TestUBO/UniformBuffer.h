#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <stdint.h>

#include <cstdint>
#include <string>
#include <type_traits>

class UniformBuffer {
    uint32_t mBufferId;

    uint32_t mBindingPoint;

    uint32_t mBufferSize;

    std::string mBindingName;

    bool mBufferDataUpdateFrequently{false};

    UniformBuffer(const uint32_t bindingPoint, const size_t bufferSize, const bool bufferDataUpdateFrequently = false);

public:
    UniformBuffer() = default;

    void Bind() const;

    void Unbind() const;

    void UniformBuffer::SetData(const void* data, const size_t size)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template<typename T>
    typename std::enable_if<std::is_trivial<T>::value, void>::type SetData(const T& data)
    {
        SetData(&data, sizeof(T));
    }

    void UniformBuffer::CleanUp()
    {
        glDeleteBuffers(1, &mBufferId);
    }

    static UniformBuffer
    CreateUniformBuffer(GLuint shaderProgram, const char* blockName, GLuint bindingPoint, GLsizeiptr size, bool dynamic)
    {
        GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, blockName);
        glUniformBlockBinding(shaderProgram, blockIndex, bindingPoint);

        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        return UniformBuffer(buffer);
    }

    // void SetData(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    // {
    //     glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    //     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
    //     glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    //     glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    //     glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // }

    // void SetData(const glm::vec4& color)
    // {
    //     glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    //     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(color));
    //     glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // }

private:
    UniformBuffer(GLuint buffer)
        : buffer(buffer)
    {
    }

    GLuint buffer;
};
