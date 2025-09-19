#include "IndexBufferObject.h"

#include "Core/GameCore/LoggerExtension.h"

#include <gl/glew.h>

using namespace EngineCore;

namespace Graphics {
namespace OpenGL {
IndexBufferObject::IndexBufferObject(const std::vector<uint32_t>& data, eDataCarryFlag dataCarryFlag)
    : VertexBufferObjectBase("PositionIndex", GL_ELEMENT_ARRAY_BUFFER)
    , m_data(data)
    , m_dataCarryFlag(dataCarryFlag)
    , m_countOfIndices(m_data.size())
    , m_countOfTotalLengthOfData(m_countOfIndices)
{
    LogInfo("IndexBufferObject::ctor");
}

IndexBufferObject::~IndexBufferObject()
{
    LogInfo("IndexBufferObject::~dctor");
}

void IndexBufferObject::GenIndexBuffer()
{
    GenBuffer();
}

void IndexBufferObject::BindIndexBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_descriptor);
}

size_t IndexBufferObject::GetElementByteSize() const
{
    return sizeof(uint32_t);
}

void IndexBufferObject::SendDataToGPU()
{
    GenIndexBuffer();
    BindIndexBuffer();
    m_allocatedBufferSize = sizeof(uint32_t) * m_countOfIndices;

    LogInfo("IndexBufferObject::SendDataToGPU: bufferSize = ", m_allocatedBufferSize);
    glBufferData(m_bufferTarget, m_allocatedBufferSize, m_data.data(), GL_STATIC_DRAW);

    // If data on CPU is unnecessary
    if (m_dataCarryFlag == eDataCarryFlag::INVALIDATE) {
        m_data.clear();
    }
}

void IndexBufferObject::UnbindIndexBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBufferObject::CleanUp()
{
    LogInfo("IndexBufferObject::CleanUp: descriptor = ", m_descriptor);
    glDeleteBuffers(1, &m_descriptor);
}

size_t IndexBufferObject::GetCountOfIndices() const
{
    return m_countOfIndices;
}

size_t IndexBufferObject::GetVectorSize() const
{
    return 1;
}

size_t IndexBufferObject::GetTotalLengthOfData() const
{
    return m_countOfTotalLengthOfData;
}

size_t IndexBufferObject::GetVertexAttribIndex() const
{
    return (size_t)-1;
}
} // namespace OpenGL
} // namespace Graphics
