#include "AnimatedSkin.h"

#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;

namespace Graphics
{
	namespace Mesh
	{

		AnimatedSkin::AnimatedSkin(const VertexArrayObject &vao, std::shared_ptr<AnimatedMeshData> animatedMeshData, const BoundingBox &boundingBox)
			: Skin(vao, boundingBox), m_animatedMeshData(animatedMeshData)
		{
			Logger::Out("AnimatedSkin::ctor. RootNode name=", animatedMeshData->RootNode->Name);
		}

		AnimatedSkin::~AnimatedSkin()
		{
			Logger::Out("AnimatedSkin::dtor");
		}

		void AnimatedSkin::CleanUp()
		{

			Skin::CleanUp();
		}

	}
}
