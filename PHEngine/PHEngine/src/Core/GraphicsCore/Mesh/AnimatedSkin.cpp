#include "AnimatedSkin.h"
#include "Core/GameCore/LoggerExtension.h"

#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace Graphics
{
	namespace Mesh
	{

		AnimatedSkin::AnimatedSkin(const std::shared_ptr<VertexArrayObject> &vao, std::shared_ptr<AnimatedMeshData> animatedMeshData, const BoundingBox3D &boundingBox)
			: Skin(vao, boundingBox), m_animatedMeshData(animatedMeshData)
		{
			LogInfo( "AnimatedSkin::ctor => RootNode name=", animatedMeshData->RootNode->Name);
		}

		AnimatedSkin::~AnimatedSkin()
		{
			LogInfo( "AnimatedSkin::dtor");
		}

		void AnimatedSkin::CleanUp()
		{
			Skin::CleanUp();
		}

	}
}
