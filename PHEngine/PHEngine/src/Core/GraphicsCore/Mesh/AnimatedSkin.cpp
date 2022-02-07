#include "AnimatedSkin.h"

#include <TinyLogger/LogInterface.h>

namespace Graphics
{
	namespace Mesh
	{

		AnimatedSkin::AnimatedSkin(const VertexArrayObject &vao, std::shared_ptr<AnimatedMeshData> animatedMeshData, const BoundingBox &boundingBox)
			: Skin(vao, boundingBox), m_animatedMeshData(animatedMeshData)
		{
			TinyLogger::LogProxy::LogMessages("AnimatedSkin::ctor. RootNode name=", std::string(animatedMeshData->RootNode->Name));
		}

		AnimatedSkin::~AnimatedSkin()
		{
			TinyLogger::LogProxy::LogMessages("AnimatedSkin::dtor");
		}

		void AnimatedSkin::CleanUp()
		{

			Skin::CleanUp();
		}

	}
}
