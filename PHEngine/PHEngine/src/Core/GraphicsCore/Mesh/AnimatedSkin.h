#pragma once

#include "Skin.h"

using namespace Graphics::Mesh;

namespace MeshLoader {
   namespace Assimp {
      struct AnimatedMeshData;
   }
}

namespace Graphics 
{
	namespace Mesh 
	{

      using MeshLoader::Assimp::AnimatedMeshData;

		class AnimatedSkin : public Skin
		{
         std::shared_ptr<AnimatedMeshData> m_animatedMeshData;

		public:

			AnimatedSkin(const VertexArrayObject& vao, std::shared_ptr<AnimatedMeshData> animatedMeshData);

			~AnimatedSkin();

         std::shared_ptr<AnimatedMeshData> GetAnimatedMeshData() {

            return m_animatedMeshData;
         }
			
			void CleanUp() override;
		};

	}
}

