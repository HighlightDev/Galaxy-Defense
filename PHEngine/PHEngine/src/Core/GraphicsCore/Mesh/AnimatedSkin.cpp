#include "AnimatedSkin.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshData.h"

using namespace MeshLoader::Assimp;

namespace Graphics
{
	namespace Mesh
	{

		AnimatedSkin::AnimatedSkin(const VertexArrayObject& vao, std::shared_ptr<AnimatedMeshData> animatedMeshData)
			: Skin(vao)
         , m_animatedMeshData(animatedMeshData)
		{
		}

		AnimatedSkin::~AnimatedSkin()
		{
		}

		void AnimatedSkin::CleanUp() {

			Skin::CleanUp();
		}

	}
}
