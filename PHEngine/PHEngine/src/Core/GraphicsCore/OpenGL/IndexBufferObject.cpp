#include "IndexBufferObject.h"

#include <gl/glew.h>
#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace Graphics
{
	namespace OpenGL
	{
		IndexBufferObject::IndexBufferObject(const std::vector<uint32_t>& data, eDataCarryFlag dataCarryFlag)
			: VertexBufferObjectBase(eAttribArrayIndexName::POSITION_INDICES, GL_ELEMENT_ARRAY_BUFFER)
			, m_data(std::move(data))
			, m_dataCarryFlag(dataCarryFlag)
			, m_countOfIndices(m_data.size())
			, m_countOfTotalLengthOfData(m_countOfIndices)
		{
			Logger::Out("IndexBufferObject::ctor");
		}

		IndexBufferObject::~IndexBufferObject()
		{
			Logger::Out("IndexBufferObject::~dctor");
		}

		void IndexBufferObject::GenIndexBuffer()
		{
			GenBuffer();
		}

		void IndexBufferObject::BindIndexBuffer()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_descriptor);
		}

		void IndexBufferObject::SendDataToGPU()
		{
			GenIndexBuffer();
			BindIndexBuffer();
			size_t bufferSize = sizeof(size_t) * GetTotalLengthOfData();

			Logger::Out("IndexBufferObject::SendDataToGPU; bufferSize = ", bufferSize);
			glBufferData(m_bufferTarget, bufferSize, m_data.data(), GL_STATIC_DRAW);

			// If data on CPU is unnecessary
			if (m_dataCarryFlag == eDataCarryFlag::Invalidate)
			{
				m_data.clear();
			}
		}

		void IndexBufferObject::UnbindIndexBuffer()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		void IndexBufferObject::CleanUp()
		{
			Logger::Out("IndexBufferObject::CleanUp; descriptor = ", m_descriptor);
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
	}
}
