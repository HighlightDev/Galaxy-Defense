#pragma once
#include <cstddef>
#include <stdint.h>
#include <gl/glew.h>
#include <memory>
#include <vector>
#include <TinyLogger/LogInterface.h>

#include "DataCarryFlag.h"
#include "VertexBufferObjectBase.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace TinyLogger;

namespace Graphics
{
	namespace OpenGL
	{
		template <typename DataType,
				  size_t vector_size,
				  int32_t gl_type = GL_FLOAT,
				  int32_t buffer_usage = GL_STREAM_DRAW,
				  int32_t attrib_divisor = 1>
		class InstancedVertexBufferObject : public VertexBufferObjectBase
		{
		private:
			static constexpr size_t m_vectorSize = vector_size;
			static constexpr int32_t m_glType = gl_type;

			void SetVertexAttribPointer(const int32_t index,
										const int32_t size,
										const bool normalized,
										const int32_t stride,
										const int32_t pointer_offset) const
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

			// For current implementation pointer offset is zero
			virtual void SetVertexAttribPointerWithSpecificParams()
			{
				size_t stride = GetElementByteSize() * m_vectorSize;
				SetVertexAttribPointer(m_vertexAttribIndex, m_vectorSize, false, stride, 0);
			}

		protected:
			std::vector<DataType> m_data;
			size_t m_totalDataLength;
			int32_t m_countOfIndices;
			int32_t m_vertexAttribIndex;
			eDataCarryFlag m_dataCarryFlag;

		public:
			InstancedVertexBufferObject(const std::vector<DataType> &data,
										const eAttribArrayIndexName attribArrayIndexName,
										const int32_t bufferTarget,
										const eDataCarryFlag flag)
				: VertexBufferObjectBase(attribArrayIndexName, bufferTarget),
				  m_data(std::move(data)),
				  m_totalDataLength(m_data.size()),
				  m_countOfIndices(m_totalDataLength / m_vectorSize),
				  m_vertexAttribIndex(int32_t(attribArrayIndexName)),
				  m_dataCarryFlag(flag)
			{
				LogInfo("InstancedVertexBufferObject::ctor");
			}

			InstancedVertexBufferObject(
				const size_t indicesCount,
				const eAttribArrayIndexName attribArrayIndexName,
				const int32_t bufferTarget)
				: VertexBufferObjectBase(attribArrayIndexName, bufferTarget),
				  m_data(),
				  m_totalDataLength(indicesCount * GetElementByteSize()),
				  m_countOfIndices(indicesCount),
				  m_vertexAttribIndex(int32_t(attribArrayIndexName)),
				  m_dataCarryFlag(eDataCarryFlag::INVALIDATE)
			{
				LogInfo("InstancedVertexBufferObject::ctor => for empty VBO");
			}

			virtual ~InstancedVertexBufferObject()
			{
				LogInfo("InstancedVertexBufferObject::~dctor");
			}

			virtual void *GetData()
			{
				return m_data.data();
			}

			std::vector<DataType> &GetCastedDataRef()
			{
				return m_data;
			}

			virtual size_t GetElementByteSize() const override
			{
				return sizeof(DataType);
			}

			virtual void SendDataToGPU() override
			{
				const size_t bufferSize = GetElementByteSize() * m_totalDataLength;
				GenBuffer();
				BindVBO();

				LogInfo("InstancedVertexBufferObject::SendDataToGPU => bufferSize = ", bufferSize);

				glBufferData(m_bufferTarget, bufferSize, m_data.size() ? m_data.data() : nullptr, buffer_usage);
				glEnableVertexAttribArray(m_vertexAttribIndex);
				this->SetVertexAttribPointerWithSpecificParams();
				glVertexAttribDivisor(m_vertexAttribIndex, attrib_divisor);

				// If data on CPU is unnecessary
				if (m_data.size() && m_dataCarryFlag == eDataCarryFlag::INVALIDATE)
				{
					m_data.clear();
				}
			}

			void InvalidateData()
			{
				if (m_data.size())
				{
					m_data.clear();
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
				LogInfo("InstancedVertexBufferObject::CleanUp => descriptor = ", m_descriptor);
				UnbindVBO();
				glDeleteBuffers(1, &m_descriptor);
			}
		};
	}
}
