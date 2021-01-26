#pragma once

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"

using namespace Graphics::OpenGL;

namespace Graphics
{

	class ScreenQuad
	{

		static ScreenQuad* m_instance;

		VertexArrayObject* m_vao;

		ScreenQuad();

		void Init();

	public:

		~ScreenQuad();

		static ScreenQuad* GetInstance()
		{
			if (!m_instance)
				m_instance = new ScreenQuad();

			return m_instance;
		}

      VertexArrayObject* GetBuffer() const;
	};

}

