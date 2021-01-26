#include "ScreenQuad.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObjectBase.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/BoundingBoxBuilder.h"

#include <memory>
#include <vector>
#include <utility>

using namespace Common;
using namespace Resources;
using namespace Game;

namespace Graphics
{

	ScreenQuad* ScreenQuad::m_instance = nullptr;

	ScreenQuad::ScreenQuad()
      : m_vao(new VertexArrayObject())
	{
		Init();
	}

	ScreenQuad::~ScreenQuad()
	{
      delete m_vao;
	}

   VertexArrayObject* ScreenQuad::GetBuffer() const
   {
      return m_vao;
   }

	void ScreenQuad::Init()
	{
      const auto folderManager = IO::FolderManager::GetInstance();

		/*Screen fill quad*/
		std::vector<float> vertices = { -1.0f, -1.0f, 0.0f ,
			 1.0f, -1.0f, 0.0f ,
			 1.0f, 1.0f, 0.0f ,
			 1.0f, 1.0f, 0.0f ,
			-1.0f, 1.0f, 0.0f ,
			-1.0f, -1.0f, 0.0f };

		std::vector<float> texCoords = { 0, 1 ,
			 1, 1 ,
			 1, 0 ,
			 1, 0 ,
			 0, 0 ,
			 0, 1 };

      VertexBufferObject< float, 3, GL_FLOAT>* verticesVBO = new VertexBufferObject< float, 3, GL_FLOAT>(vertices, GL_ARRAY_BUFFER, 0, DataCarryFlag::Invalidate);
		VertexBufferObjectBase* texCoordsVBO = new VertexBufferObject< float, 2, GL_FLOAT>(texCoords, GL_ARRAY_BUFFER, 2, DataCarryFlag::Invalidate);

		m_vao->AddVBO(verticesVBO, texCoordsVBO);

		m_vao->BindBuffersToVao();
	}
}
