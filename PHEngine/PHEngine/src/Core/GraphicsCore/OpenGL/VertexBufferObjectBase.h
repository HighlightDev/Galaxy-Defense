#pragma once
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

		public:
			VertexBufferObjectBase(int32_t bufferTarget = GL_ARRAY_BUFFER);
			virtual ~VertexBufferObjectBase();

			void GenBuffer();

			void BindVBO();

			static void UnbindVBO();

			virtual void SendDataToGPU() = 0;

			virtual size_t GetCountOfIndices() const = 0;

			virtual size_t GetTotalLengthOfData() const = 0;

			virtual size_t GetVectorSize() const = 0;

			virtual size_t GetVertexAttribIndex() const = 0;

			virtual void CleanUp() = 0;

			virtual void* GetData();
		};
	}
}

