#include "VertexArrayObject.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"

namespace Graphics
{
	namespace OpenGL
	{
		VertexArrayObject::VertexArrayObject()
			: m_ibo(nullptr)
		{
			LogInfo( "VertexArrayObject::ctor");
			GenVAO();
		}

		VertexArrayObject::~VertexArrayObject()
		{
			LogInfo( "VertexArrayObject::~dctor");
		}

		bool VertexArrayObject::HasIBO() const
		{
			return m_ibo != nullptr;
		}

		VertexBufferObjectBase *VertexArrayObject::GetVboByIndex(const size_t index) const
		{
			return m_vbos[index].first;
		}

		const std::vector<std::pair<VertexBufferObjectBase *, eAttribArrayIndexName>> &VertexArrayObject::GetVertexBufferObjects() const
		{
			return m_vbos;
		}

		void VertexArrayObject::GenVAO()
		{
			assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));
			glGenVertexArrays(1, &m_descriptor);
			LogInfo( "VertexArrayObject::GenVAO => descriptor = ", m_descriptor);
		}

		void VertexArrayObject::RenderVAO(const int32_t primitiveMode)
		{
			glBindVertexArray(m_descriptor);
			if (HasIBO())
			{
				glDrawElements(primitiveMode, m_ibo->GetCountOfIndices(), GL_UNSIGNED_INT, 0);
			}
			else
			{
				VertexBufferObjectBase *positionVBO = GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
				assert(positionVBO);
				glDrawArrays(primitiveMode, 0, positionVBO->GetCountOfIndices());
			}
			glBindVertexArray(0);
		}

		void VertexArrayObject::RenderVAO(const size_t first, const size_t count, const int32_t primitiveMode)
		{
			glBindVertexArray(m_descriptor);
			if (HasIBO())
			{
				glDrawElements(primitiveMode, count, GL_UNSIGNED_INT, 0);
			}
			else
			{
				glDrawArrays(primitiveMode, first, count);
			}
			glBindVertexArray(0);
		}

		void VertexArrayObject::RenderInstanced(const int32_t primitiveMode, const size_t primitivesCount)
		{
			glBindVertexArray(m_descriptor);
			VertexBufferObjectBase *positionVBO = GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
			assert(positionVBO);
			glDrawArraysInstanced(primitiveMode, 0, positionVBO->GetCountOfIndices(), primitivesCount);
			glBindVertexArray(0);
		}

		void VertexArrayObject::AddIndexBuffer(IndexBufferObject *ibo)
		{
			LogInfo( "VertexArrayObject::AddIndexBuffer => descriptor = ", m_descriptor,
						"IBO descriptor = ", ibo->GetDescriptor());
			m_ibo = ibo;
		}

		void VertexArrayObject::BindBuffersToVao()
		{
			LogInfo( "VertexArrayObject::BindBuffersToVao => descriptor = ", m_descriptor);

			glBindVertexArray(m_descriptor);

			if (m_ibo)
				m_ibo->SendDataToGPU();

			for (auto it = m_vbos.begin(); it != m_vbos.end(); ++it)
			{
				it->first->SendDataToGPU();
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
				glDisableVertexAttribArray(it->first->GetVertexAttribIndex());
			}
		}

		void VertexArrayObject::CleanUp()
		{
			LogInfo( "VertexArrayObject::CleanUp => descriptor = ", m_descriptor);

			glBindVertexArray(0);

			if (m_ibo)
				m_ibo->CleanUp();

			for (auto it = m_vbos.begin(); it != m_vbos.end(); ++it)
			{
				it->first->CleanUp();
			}

			glDeleteVertexArrays(1, &m_descriptor);

			const size_t vbos_size = m_vbos.size();
			for (size_t i = 0; i < vbos_size; ++i)
			{
				delete m_vbos[i].first;
			}
			m_vbos.clear();

			delete m_ibo;
		}
	}
}
