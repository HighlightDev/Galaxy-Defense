#pragma once

#include "VertexBufferObject.h"
#include "VertexBufferObjectBase.h"
#include "IndexBufferObject.h"

#include <vector>
#include <memory>
#include <gl\glew.h>

namespace Graphics
{
	namespace OpenGL
	{
		class VertexArrayObject
		{
		private:

			uint32_t m_descriptor;
			std::vector<std::unique_ptr<VertexBufferObjectBase>> m_vbos;
			std::unique_ptr<IndexBufferObject> m_ibo;

		public:

			VertexArrayObject();
			~VertexArrayObject();


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

			std::vector<std::unique_ptr<VertexBufferObjectBase>>& GetVertexBufferObjects();

			void GenVAO();

			void RenderVAO(int32_t primitiveMode = GL_TRIANGLES);

			void AddIndexBuffer(std::unique_ptr<IndexBufferObject>&& ibo);

			void BindBuffersToVao();

			void DisableVertexAttribArrays();

			void CleanUp();
		};
	}
}

