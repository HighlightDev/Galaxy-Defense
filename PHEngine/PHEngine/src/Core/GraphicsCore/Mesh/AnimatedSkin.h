#pragma once

#include "Skin.h"
#include "AnimatedMeshData.h"

namespace Graphics 
{
	namespace Mesh 
	{

		class AnimatedSkin : public Skin
		{
         std::shared_ptr<AnimatedMeshData> m_animatedMeshData;

		public:

			AnimatedSkin(const VertexArrayObject& vao, std::shared_ptr<AnimatedMeshData> animatedMeshData, const BoundingBox& boundingBox);

			~AnimatedSkin();

         std::shared_ptr<AnimatedMeshData> GetAnimatedMeshData() {

            return m_animatedMeshData;
         }
			
			void CleanUp() override;
		};

	}
}

