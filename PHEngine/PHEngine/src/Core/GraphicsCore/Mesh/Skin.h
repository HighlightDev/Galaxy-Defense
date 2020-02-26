#pragma once

#include <memory>
#include <utility>

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"

using namespace Graphics::OpenGL;

namespace Graphics
{
	namespace Mesh
	{

		class Skin
		{

			std::unique_ptr<VertexArrayObject> m_buffer;

		public:

			Skin(std::unique_ptr<VertexArrayObject>&& vao);

			virtual ~Skin();

			inline std::unique_ptr<VertexArrayObject>& GetBuffer() {

				return m_buffer;
			}

			virtual void CleanUp();
		};

	}
}

