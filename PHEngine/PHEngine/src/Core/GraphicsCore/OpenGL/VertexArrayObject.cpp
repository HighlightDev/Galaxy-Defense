#include "VertexArrayObject.h"

namespace Graphics
{
	namespace OpenGL
	{
		VertexArrayObject::VertexArrayObject()
			: m_ibo(nullptr)
		{
			GenVAO();
		}

		VertexArrayObject::~VertexArrayObject()
		{
		}

		bool VertexArrayObject::HasIBO() const
		{
			return m_ibo != nullptr;
		}

		VertexBufferObjectBase *VertexArrayObject::GetVboByIndex(const size_t index) const
		{
			return m_vbos[index];
		}

		const std::vector<VertexBufferObjectBase *> &VertexArrayObject::GetVertexBufferObjects() const
		{
			return m_vbos;
		}

		void VertexArrayObject::GenVAO()
		{
			glGenVertexArrays(1, &m_descriptor);
		}

		void VertexArrayObject::RenderVAO(int32_t primitiveMode)
		{
			glBindVertexArray(m_descriptor);
			if (HasIBO())
			{
				glDrawElements(primitiveMode, m_ibo->GetCountOfIndices(), GL_UNSIGNED_INT, 0);
			}
			else
			{
				glDrawArrays(primitiveMode, 0, m_vbos.front()->GetCountOfIndices());
			}
			glBindVertexArray(0);
		}

		void VertexArrayObject::AddIndexBuffer(IndexBufferObject *ibo)
		{
			m_ibo = ibo;
		}

		void VertexArrayObject::BindBuffersToVao()
		{
			glBindVertexArray(m_descriptor);

			if (m_ibo)
				m_ibo->SendDataToGPU();

			for (auto it = m_vbos.begin(); it != m_vbos.end(); ++it)
			{
				(*it)->SendDataToGPU();
			}
			glBindVertexArray(0);
			DisableVertexAttribArrays();
		}

		void VertexArrayObject::DisableVertexAttribArrays()
		{
			IndexBufferObject::UnbindIndexBuffer();
			VertexBufferObjectBase::UnbindVBO();
			for (auto it = m_vbos.begin(); it != m_vbos.end(); ++it)
			{
				glDisableVertexAttribArray((*it)->GetVertexAttribIndex());
			}
		}

		void VertexArrayObject::CleanUp()
		{
			if (m_ibo)
				m_ibo->CleanUp();

			for (auto it = m_vbos.begin(); it != m_vbos.end(); ++it)
			{
				(*it)->CleanUp();
			}

			glDeleteVertexArrays(1, &m_descriptor);

			for (size_t i = 0; i < m_vbos.size(); ++i)
			{
				delete m_vbos[i];
			}
			m_vbos.clear();

			delete m_ibo;
		}
	}
}
