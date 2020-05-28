#include "AnimatedSkin.h"

namespace Graphics
{
	namespace Mesh
	{

		AnimatedSkin::AnimatedSkin(const VertexArrayObject& vao, std::shared_ptr<Bone> rootBone, std::shared_ptr<AnimatedMeshData> animatedMeshData)
			: Skin(vao)
			, m_rootBone(rootBone)
         , m_animatedMeshData(animatedMeshData)
		{
		}


		AnimatedSkin::~AnimatedSkin()
		{
		}

		void AnimatedSkin::CleanUp() {

			Skin::CleanUp();
			m_rootBone->CleanUp();
		}

	}
}
