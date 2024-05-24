#pragma once

#include <string>
#include <cstddef>
#include <stdint.h>
#include <gl/glew.h>

namespace Graphics
{
	namespace OpenGL
	{
		class VertexBufferObjectBase
		{
		protected:
			uint32_t m_descriptor;

			int32_t m_bufferTarget;

			std::string mAttribArrayIndexName;

			size_t m_allocatedBufferSize{0};

		public:
			VertexBufferObjectBase(const std::string& attribArrayIndexName, const int32_t bufferTarget = GL_ARRAY_BUFFER);
			virtual ~VertexBufferObjectBase();

			void GenBuffer();

			void BindVBO();

			static void UnbindVBO();

			virtual void SendDataToGPU() = 0;

			virtual size_t GetCountOfIndices() const = 0;

			virtual size_t GetTotalLengthOfData() const = 0;

			virtual size_t GetVectorSize() const = 0;

			virtual size_t GetVertexAttribIndex() const = 0;

			virtual size_t GetElementByteSize() const = 0;

			virtual void BufferSubData(const size_t offset, const size_t size, const void *data) const;

			virtual void CleanUp() = 0;

			virtual void *GetData();

			size_t GetDescriptor() const;

			std::string GetAttribArrayIndexName() const;

			int32_t GetBufferTarget() const;

			size_t GetAllocatedBufferSize() const;
		};
	}
}
