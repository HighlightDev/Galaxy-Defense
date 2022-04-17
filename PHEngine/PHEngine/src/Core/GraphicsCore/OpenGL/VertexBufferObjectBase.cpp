#include "VertexBufferObjectBase.h"

namespace Graphics
{
	namespace OpenGL
	{
		VertexBufferObjectBase::VertexBufferObjectBase(const eAttribArrayIndexName attribArrayIndexName,
													   const int32_t bufferTarget)
			: m_bufferTarget(bufferTarget)
			, mAttribArrayIndexName(attribArrayIndexName)
		{
		}

		VertexBufferObjectBase::~VertexBufferObjectBase()
		{
		}

		void VertexBufferObjectBase::GenBuffer()
		{
			glGenBuffers(1, &m_descriptor);
		}

		void VertexBufferObjectBase::BindVBO()
		{
			glBindBuffer(m_bufferTarget, m_descriptor);
		}

		void VertexBufferObjectBase::UnbindVBO()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void *VertexBufferObjectBase::GetData()
		{
			return nullptr;
		}

		size_t VertexBufferObjectBase::GetDescriptor() const
		{
			return m_descriptor;
		}

		eAttribArrayIndexName VertexBufferObjectBase::GetAttribArrayIndexName() const
		{
			return mAttribArrayIndexName;
		}
	}
}
