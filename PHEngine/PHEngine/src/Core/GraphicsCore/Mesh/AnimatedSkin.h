#pragma once

#include "Skin.h"
#include "Core/GraphicsCore/Animation/Bone.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshData.h"

using namespace Graphics::Mesh;
using namespace Graphics::Animation;

using namespace IO::MeshLoader::Assimp;

namespace Graphics 
{
	namespace Mesh 
	{

		class AnimatedSkin : public Skin
		{
			std::shared_ptr<Bone> m_rootBone;

         std::shared_ptr<AnimatedMeshData> m_animatedMeshData;

		public:

			AnimatedSkin(const VertexArrayObject& vao, std::shared_ptr<Bone> rootBone, std::shared_ptr<AnimatedMeshData> animatedMeshData);

			~AnimatedSkin();

			std::shared_ptr<Bone>& GetRootBone() {

				return m_rootBone;
			}

         std::shared_ptr<AnimatedMeshData> GetAnimatedMeshData() {

            return m_animatedMeshData;
         }
			
			void CleanUp() override;
		};

	}
}

