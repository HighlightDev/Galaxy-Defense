#include "AnimationAllocationPolicy.h"

#include "Core/UtilityCore/AssimpSkeletonConverter.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/IoCore/MeshLoaderCore/MeshResourceInfo.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/RawResource.h"
#include "Core/CommonCore/Assertion.h"

using namespace IO;
using namespace IO::MeshLoader::Assimp;
using namespace EngineUtility;

namespace Resources
{
   template class AnimationAllocationPolicy<std::string>;

   template <typename Model>
   std::shared_ptr<std::vector<AnimationSequence>> AnimationAllocationPolicy<Model>::AllocateMemory(Model& arg)
   {
      const int32_t countOfBonesInfluencingOnVertex = 3;

      std::vector<AnimationSequence> resultAnimationCollection;
    
      Resource* outResource;
      bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg);

      assert(bResourceValid);

      MeshResource* meshResource = static_cast<MeshResource*>(outResource);
      MeshResourceInfo* meshInfo = meshResource->GetMeshResourceInfo();

      if (meshInfo->MeshAnimatedData)
      {
         AnimationData* animationData = meshInfo->AninationData;
         resultAnimationCollection = EngineUtility::AssimpSkeletonConverter::ConvertAssimpAnimationToEngineAnimation(animationData->GetAnimations());
      }

      return std::make_shared<std::vector<AnimationSequence>>(std::move(resultAnimationCollection));
   }

   template <typename Model>
   void AnimationAllocationPolicy<Model>::DeallocateMemory(std::shared_ptr<std::vector<AnimationSequence>> arg)
   {
   }

}