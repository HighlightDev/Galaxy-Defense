#pragma once
#include <cstddef>
#include <stdint.h>
#include <gl/glew.h>
#include <memory>
#include <vector>

#include "DataCarryFlag.h"
#include "VertexBufferObjectBase.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace Graphics
{
	namespace OpenGL
	{
		template <typename DataType, int32_t buffer_usage = GL_STATIC_DRAW>
		class VertexBufferObject : public VertexBufferObjectBase
		{
		protected:
			std::vector<DataType> m_data;
			int32_t m_vectorSize;
			size_t m_totalDataLength;
			int32_t m_countOfIndices;
			int32_t m_vertexAttribIndex;
			int32_t m_glType;
			eDataCarryFlag m_dataCarryFlag;

		public:
			VertexBufferObject(const std::vector<DataType> &data,
							   const std::string &attribArrayIndexName,
							   const int32_t attributeIndex,
							   const int32_t glType,
							   const int32_t vectorSize,
							   const int32_t bufferTarget,
							   const eDataCarryFlag flag)
				: VertexBufferObjectBase(attribArrayIndexName, bufferTarget),
				  m_data(std::move(data)),
				  m_vectorSize(vectorSize),
				  m_totalDataLength(m_data.size()),
				  m_countOfIndices(m_totalDataLength / m_vectorSize),
				  m_vertexAttribIndex(attributeIndex),
				  m_glType(glType),
				  m_dataCarryFlag(flag)
			{
				LogInfo("VertexBufferObject::ctor");
			}

			VertexBufferObject(
				const size_t indicesCount,
				const std::string &attribArrayIndexName,
				const int32_t attributeIndex,
				const int32_t glType,
				const int32_t vectorSize,
				const int32_t bufferTarget)
				: VertexBufferObjectBase(attribArrayIndexName, bufferTarget),
				  m_countOfIndices(indicesCount),
				  m_vertexAttribIndex(attributeIndex),
				  m_vectorSize(vectorSize),
				  m_totalDataLength(indicesCount * vectorSize),
				  m_glType(glType),
				  m_dataCarryFlag(eDataCarryFlag::INVALIDATE)
			{
				LogInfo("VertexBufferObject::ctor => for empty VBO");
			}

			virtual ~VertexBufferObject()
			{
			}

		private:
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

		public:
			virtual void *GetData()
			{
				return m_data.data();
			}

			std::vector<DataType> &GetCastedDataRef()
			{
				return m_data;
			}

			size_t GetElementByteSize() const override
			{
				return sizeof(DataType);
			}

			void SendDataToGPU() override
			{
				m_allocatedBufferSize = GetElementByteSize() * m_totalDataLength;
				GenBuffer();
				BindVBO();

				LogInfo("VertexBufferObject::SendDataToGPU => descriptor = ", m_descriptor, " bufferSize = ", m_allocatedBufferSize);

				glBufferData(m_bufferTarget, m_allocatedBufferSize, m_data.size() ? m_data.data() : nullptr, buffer_usage);
				glEnableVertexAttribArray(m_vertexAttribIndex);
				this->SetVertexAttribPointerWithSpecificParams();

				// If data on CPU is unnecessary
				if (m_dataCarryFlag == eDataCarryFlag::INVALIDATE)
				{
					m_data.clear();
				}
			}

			void InvalidateData()
			{
				m_data.clear();
			}

			size_t GetCountOfIndices() const override
			{
				return m_countOfIndices;
			}

			size_t GetTotalLengthOfData() const override
			{
				return m_totalDataLength;
			}

			size_t GetVectorSize() const override
			{
				return m_vectorSize;
			}

			size_t GetVertexAttribIndex() const override
			{
				return m_vertexAttribIndex;
			}

			void CleanUp() override
			{
				LogInfo("VertexBufferObject::CleanUp => descriptor = ", m_descriptor);
				UnbindVBO();
				glDeleteBuffers(1, &m_descriptor);
			}
		};
	}
}
