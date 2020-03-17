#include "AnimatedSkin.h"

namespace Graphics
{
	namespace Mesh
	{

		AnimatedSkin::AnimatedSkin(const VertexArrayObject& vao, std::shared_ptr<Bone> rootBone)
			: Skin(vao)
			, m_rootBone(rootBone)
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
