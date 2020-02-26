#pragma once

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GameCore/ShaderImplementation/ResolveTextureShader.h"

using namespace Graphics::OpenGL;
using namespace Game::ShaderImpl;

namespace Graphics
{

	class ScreenQuad
	{

		static ScreenQuad* m_instance;

      std::shared_ptr<ResolveTextureShader> m_resolveTexShader;

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

		inline VertexArrayObject* GetBuffer() const
		{
			return m_vao;
		}

      inline std::shared_ptr<ResolveTextureShader> GetResolveTexShader() const
      {
         return m_resolveTexShader;
      }

	};

}

