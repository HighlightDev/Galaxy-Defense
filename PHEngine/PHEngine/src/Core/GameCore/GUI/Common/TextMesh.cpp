#include "TextMesh.h"

namespace Graphics {
namespace Mesh {

TextMesh::TextMesh(const VertexArrayObject& vao)
    : m_buffer(vao)
{
}

TextMesh::~TextMesh()
{
}

bool TextMesh::operator==(const TextMesh& right) const
{
    return this->m_buffer.GetDescriptor() == right.m_buffer.GetDescriptor();
}

VertexArrayObject* TextMesh::GetBuffer()
{
    return &m_buffer;
}

void TextMesh::CleanUp()
{
    m_buffer.CleanUp();
}
} // namespace Mesh
} // namespace Graphics
