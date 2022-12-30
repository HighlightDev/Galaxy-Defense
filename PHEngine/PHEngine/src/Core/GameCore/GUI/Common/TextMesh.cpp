#include "TextMesh.h"

namespace Graphics
{
   namespace Mesh
   {

      TextMesh::TextMesh(const VertexArrayObject &vao)
          : m_buffer(vao)
      {
      }

      TextMesh::~TextMesh()
      {
      }

      VertexArrayObject *TextMesh::GetBuffer()
      {
         return &m_buffer;
      }

      void TextMesh::CleanUp()
      {
         m_buffer.CleanUp();
      }
   }
}
