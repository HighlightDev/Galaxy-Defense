#pragma once

#include "Skin.h"
#include "Core/GraphicsCore/Animation/Bone.h"

using namespace Graphics::Mesh;
using namespace Graphics::Animation;

namespace Graphics 
{
	namespace Mesh 
	{

		class AnimatedSkin : public Skin
		{
			std::shared_ptr<Bone> m_rootBone;

		public:

			AnimatedSkin(std::unique_ptr<VertexArrayObject> vao, std::shared_ptr<Bone> rootBone);

			~AnimatedSkin();

			std::shared_ptr<Bone>& GetRootBone() {

				return m_rootBone;
			}
			
			void CleanUp() override;
		};

	}
}

