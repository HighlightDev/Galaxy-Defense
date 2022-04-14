#pragma once
#include <cstddef>
#include <stdint.h>
#include <gl/glew.h>
#include <memory>
#include <vector>
#include <TinyLogger/LogInterface.h>

#include "DataCarryFlag.h"
#include "VertexBufferObjectBase.h"

using namespace TinyLogger;

namespace Graphics
{
	namespace OpenGL
	{
		template <typename DataType, size_t vector_size, int32_t gl_type = GL_FLOAT>
		class VertexBufferObject : public VertexBufferObjectBase
		{
		private:
			static constexpr size_t m_vectorSize = vector_size;
			static constexpr int32_t m_glType = gl_type;

			void SetVertexAttribPointer(int32_t index, int32_t size, bool normalized, int32_t stride, int32_t pointer_offset) const
			{
				if (m_glType == GL_INT ||
					m_glType == GL_UNSIGNED_BYTE ||
					m_glType == GL_UNSIGNED_INT)
				{
					glVertexAttribIPointer(index, size, m_glType, stride, 0);
				}
				else if (m_glType == GL_DOUBLE)
				{
					glVertexAttribLPointer(index, size, m_glType, stride, 0);
				}
				else if (m_glType == GL_FLOAT)
				{
					glVertexAttribPointer(index, size, m_glType, normalized, stride, 0);
				}
			}

			static const size_t GetVectorElementByteSize()
			{
				return sizeof(DataType);
			}

			// For current implementation pointer offset is zero
			virtual void SetVertexAttribPointerWithSpecificParams()
			{
				size_t stride = GetVectorElementByteSize() * m_vectorSize;
				SetVertexAttribPointer(m_vertexAttribIndex, m_vectorSize, false, stride, 0);
			}

		protected:
			std::vector<DataType> m_data;
			size_t m_totalDataLength;
			int32_t m_countOfIndices;
			int32_t m_vertexAttribIndex;
			DataCarryFlag m_dataCarryFlag;

		public:
			VertexBufferObject(const std::vector<DataType> &data, int32_t bufferTarget, int32_t vertexAttribIndex, DataCarryFlag flag)
				: VertexBufferObjectBase(bufferTarget),
				  m_data(std::move(data)),
				  m_totalDataLength(m_data.size()),
				  m_countOfIndices(m_totalDataLength / m_vectorSize),
				  m_vertexAttribIndex(vertexAttribIndex),
				  m_dataCarryFlag(flag)
			{
				Logger::Out("VertexBufferObject::ctor");
			}

			virtual ~VertexBufferObject()
			{
				Logger::Out("VertexBufferObject::~dctor");
			}

			virtual void *GetData()
			{
				return m_data.data();
			}

			std::vector<DataType> &GetCastedDataRef()
			{
				return m_data;
			}

			virtual void SendDataToGPU() override
			{
				const size_t bufferSize = GetVectorElementByteSize() * m_totalDataLength;
				GenBuffer();
				BindVBO();

				Logger::Out("VertexBufferObject::SendDataToGPU; bufferSize = ", bufferSize);

				glBufferData(m_bufferTarget, bufferSize, m_data.data(), GL_STATIC_DRAW);
				glEnableVertexAttribArray(m_vertexAttribIndex);
				this->SetVertexAttribPointerWithSpecificParams();

				// If data on CPU is unnecessary
				if (m_dataCarryFlag == DataCarryFlag::Invalidate)
				{
					m_data.clear();
				}
			}

			void InvalidateData()
			{
				m_data.clear();
			}

			virtual size_t GetCountOfIndices() const override
			{
				return m_countOfIndices;
			}

			virtual size_t GetTotalLengthOfData() const override
			{
				return m_totalDataLength;
			}

			virtual size_t GetVectorSize() const override
			{
				return m_vectorSize;
			}

			virtual size_t GetVertexAttribIndex() const override
			{
				return m_vertexAttribIndex;
			}

			virtual void CleanUp() override
			{
				Logger::Out("VertexBufferObject::CleanUp; descriptor = ", m_descriptor);
				UnbindVBO();
				glDeleteBuffers(1, &m_descriptor);
			}
		};
	}
}
