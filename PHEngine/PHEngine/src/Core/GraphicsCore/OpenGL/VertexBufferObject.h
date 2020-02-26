#pragma once
#include <stdint.h>
#include <GL/glew.h>
#include <memory>
#include <vector>

#include "DataCarryFlag.h"
#include "VertexBufferObjectBase.h"

namespace Graphics
{
	namespace OpenGL
	{
		template <typename DataType, int32_t vector_size, int32_t gl_type = GL_FLOAT>
		class VertexBufferObject : public VertexBufferObjectBase
		{
		private:

			static constexpr int32_t m_vectorSize = vector_size;
			static constexpr int32_t m_glType = gl_type;

			static void SetVertexAttribPointer(int32_t index, int32_t size, bool normalized, int32_t stride, int32_t pointer_offset)
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

			std::shared_ptr<std::vector<DataType>> m_data;
			size_t m_totalDataLength;
			int32_t m_countOfIndices;
			int32_t m_vertexAttribIndex;
			DataCarryFlag m_dataCarryFlag;

		public:

			VertexBufferObject(std::shared_ptr<std::vector<DataType>> data, int32_t bufferTarget, int32_t vertexAttribIndex, DataCarryFlag flag)
				: VertexBufferObjectBase(bufferTarget)
				, m_data(data)
				, m_totalDataLength(data->size())
				, m_countOfIndices(m_totalDataLength / m_vectorSize)
				, m_vertexAttribIndex(vertexAttribIndex)
				, m_dataCarryFlag(flag)
			{
			}

			virtual ~VertexBufferObject()
			{
			}

			virtual void* GetData()
			{
				return &m_data;
			}

			std::shared_ptr<std::vector<DataType>> GetData() const
			{
				return m_data;
			}

			virtual void SendDataToGPU() override
			{
				const size_t data_size = GetVectorElementByteSize() * m_totalDataLength;
				GenBuffer();
				BindVBO();
				
				glBufferData(m_bufferTarget, data_size, m_data->data(), GL_STATIC_DRAW);
				glEnableVertexAttribArray(m_vertexAttribIndex);
				this->SetVertexAttribPointerWithSpecificParams();

				// If data on CPU is unnecessary
				if (m_dataCarryFlag == DataCarryFlag::Invalidate)
				{
					m_data = nullptr;
					m_data.~shared_ptr();
				}
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
				glDeleteBuffers(1, &m_descriptor);
			}
		};
	}
}


