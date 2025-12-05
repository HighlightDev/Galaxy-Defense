#include "VertexArrayObject.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"

namespace Graphics {
namespace OpenGL {
VertexArrayObject::VertexArrayObject()
    : m_ibo(nullptr)
{
    GenVAO();
}

VertexArrayObject::~VertexArrayObject()
{
}

uint32_t VertexArrayObject::GetDescriptor() const
{
    return m_descriptor;
}

bool VertexArrayObject::HasIBO() const
{
    return m_ibo != nullptr;
}

void VertexArrayObject::SetIBO(IndexBufferObject* const ibo)
{
    if (m_ibo != nullptr) {
        m_ibo->CleanUp();
        delete m_ibo;
    }
    m_ibo = ibo;
}

const IndexBufferObject* VertexArrayObject::GetIBO() const
{
    return m_ibo;
}

VertexBufferObjectBase* VertexArrayObject::GetVBOByIndex(const size_t index) const
{
    return m_vbos[index].first;
}

const std::vector<std::pair<VertexBufferObjectBase*, std::string>>& VertexArrayObject::GetVBOs() const
{
    return m_vbos;
}

void VertexArrayObject::GenVAO()
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"), "GenVAO must be called from Render thread");
    glGenVertexArrays(1, &m_descriptor);
}

void VertexArrayObject::RenderVAO(const int32_t primitiveMode)
{
    glBindVertexArray(m_descriptor);
    if (HasIBO()) {
        glDrawElements(primitiveMode, m_ibo->GetCountOfIndices(), GL_UNSIGNED_INT, 0);
    } else {
        VertexBufferObjectBase* positionVBO = GetVboByAttribArrayIndexName("VertexPosition");
        ext_assert(positionVBO, "Position VBO is required for rendering");
        glDrawArrays(primitiveMode, 0, positionVBO->GetCountOfIndices());
    }
    glBindVertexArray(0);
}

void VertexArrayObject::RenderVAO(const size_t first, const size_t count, const int32_t primitiveMode)
{
    glBindVertexArray(m_descriptor);
    if (HasIBO()) {
        glDrawElements(primitiveMode, count, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(primitiveMode, first, count);
    }
    glBindVertexArray(0);
}

void VertexArrayObject::RenderInstanced(const int32_t primitiveMode, const size_t primitivesCount)
{
    glBindVertexArray(m_descriptor);
    VertexBufferObjectBase* positionVBO = GetVboByAttribArrayIndexName("VertexPosition");
    ext_assert(positionVBO, "Position VBO is required for instanced rendering");
    if (HasIBO()) {
        glDrawElementsInstanced(primitiveMode, m_ibo->GetCountOfIndices(), GL_UNSIGNED_INT, 0, primitivesCount);
    } else {
        glDrawArraysInstanced(primitiveMode, 0, positionVBO->GetCountOfIndices(), primitivesCount);
    }
    glBindVertexArray(0);
}

void VertexArrayObject::BindBuffersToVao()
{
    glBindVertexArray(m_descriptor);

    if (m_ibo)
        m_ibo->SendDataToGPU();

    for (auto it = m_vbos.begin(); it != m_vbos.end(); ++it) {
        it->first->SendDataToGPU();
    }
    glBindVertexArray(0);
    DisableVertexAttribArrays();
}

void VertexArrayObject::DisableVertexAttribArrays()
{
    IndexBufferObject::UnbindIndexBuffer();
    VertexBufferObjectBase::UnbindVBO();
    for (auto it = m_vbos.begin(); it != m_vbos.end(); ++it) {
        glDisableVertexAttribArray(it->first->GetVertexAttribIndex());
    }
}

void VertexArrayObject::CleanUp()
{
    glBindVertexArray(0);

    if (m_ibo)
        m_ibo->CleanUp();

    for (auto it = m_vbos.begin(); it != m_vbos.end(); ++it) {
        it->first->CleanUp();
    }

    glDeleteVertexArrays(1, &m_descriptor);

    const size_t vbos_size = m_vbos.size();
    for (size_t i = 0; i < vbos_size; ++i) {
        delete m_vbos[i].first;
    }
    m_vbos.clear();

    delete m_ibo;
}
} // namespace OpenGL
} // namespace Graphics
