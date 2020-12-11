#include "AnimatedSkin.h"

namespace Graphics
{
	namespace Mesh
	{

		AnimatedSkin::AnimatedSkin(const VertexArrayObject& vao, std::shared_ptr<AnimatedMeshData> animatedMeshData, const BoundingBox& boundingBox)
			: Skin(vao, boundingBox)
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
