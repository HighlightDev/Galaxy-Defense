#include "AnimatedSkin.h"

namespace Graphics
{
	namespace Mesh
	{

		AnimatedSkin::AnimatedSkin(std::unique_ptr<VertexArrayObject> vao, std::shared_ptr<Bone> rootBone)
			: Skin(std::move(vao))
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
