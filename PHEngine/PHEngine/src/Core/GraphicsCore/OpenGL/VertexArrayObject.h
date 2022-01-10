#pragma once

#include "VertexBufferObject.h"
#include "VertexBufferObjectBase.h"
#include "IndexBufferObject.h"

#include <vector>
#include <memory>
#include <gl/glew.h>

namespace Graphics
{
	namespace OpenGL
	{
		class VertexArrayObject
		{
		private:

			uint32_t m_descriptor;
			std::vector<VertexBufferObjectBase*> m_vbos;
			IndexBufferObject* m_ibo;

		public:

			VertexArrayObject();
			~VertexArrayObject();

         VertexBufferObjectBase* GetVboByIndex(const size_t index) const;

			template <typename Arg, typename ...Args>
			void AddVBO(Arg&& p_vbo, Args... p_vbos)
			{
				if (p_vbo != nullptr)
				{
					m_vbos.emplace_back(std::forward<Arg>(p_vbo));
				}
				AddVBO(std::forward<Args>(p_vbos)...);
			}

			template <typename Arg>
			void AddVBO(Arg&& p_vbo)
			{
				if (p_vbo != nullptr)
				{
					m_vbos.emplace_back(std::forward<Arg>(p_vbo));
				}
			}

			bool HasIBO() const;

			const std::vector<VertexBufferObjectBase*>& GetVertexBufferObjects() const;

			void GenVAO();

			void RenderVAO(int32_t primitiveMode = GL_TRIANGLES);

			void AddIndexBuffer(IndexBufferObject* ibo);

			void BindBuffersToVao();

			void DisableVertexAttribArrays();

			void CleanUp();
		};
	}
}

