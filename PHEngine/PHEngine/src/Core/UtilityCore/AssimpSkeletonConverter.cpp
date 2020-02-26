#include "AssimpSkeletonConverter.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/SkeletonBoneLOADER.h"
#include "Core/GraphicsCore/Animation/AnimationFrame.h"

namespace EngineUtility
{

	std::unique_ptr<AssimpSkeletonConverter> AssimpSkeletonConverter::m_instance;

	glm::mat4 AssimpSkeletonConverter::ConvertAssimpMatrix4x4ToOpenTKMatrix4(aiMatrix4x4& srcMatrix)
	{
		glm::mat4 dstMatrix(srcMatrix.a1, srcMatrix.b1, srcMatrix.c1, srcMatrix.d1,
         srcMatrix.a2, srcMatrix.b2, srcMatrix.c2, srcMatrix.d2,
         srcMatrix.a3, srcMatrix.b3, srcMatrix.c3, srcMatrix.d3,
         srcMatrix.a4, srcMatrix.b4, srcMatrix.c4, srcMatrix.d4);

		return dstMatrix;
	}

	void AssimpSkeletonConverter::IterateBoneTree(Bone* dstParentBone, Io::MeshLoader::Assimp::SkeletonBoneLOADER* srcParentNode)
	{
		for (auto& srcNode : srcParentNode->GetChildren())
		{
         // SHOULD BE POINTER!!!
			Bone* dstChildBone = new Bone(srcNode->GetBoneId(), srcNode->GetBoneInfo()->mName.C_Str(), dstParentBone);
			dstChildBone->SetOffsetMatrix(ConvertAssimpMatrix4x4ToOpenTKMatrix4(srcNode->GetBoneInfo()->mOffsetMatrix));
			dstParentBone->AddChild(dstChildBone);
			IterateBoneTree(dstChildBone, srcNode);
		}
	}

	Bone* AssimpSkeletonConverter::ConvertAssimpBoneToEngineBone(SkeletonBoneBaseLOADER* rootBone)
	{
		Bone* resultBone = new Bone(-1, "ROOT_BONE");

		for (auto& bone : rootBone->GetChildren())
		{
			int32_t id = bone->GetBoneId();
			Bone* root = new Bone(id, bone->GetBoneInfo()->mName.C_Str());
			root->SetOffsetMatrix(ConvertAssimpMatrix4x4ToOpenTKMatrix4(bone->GetBoneInfo()->mOffsetMatrix));
			IterateBoneTree(root, bone);
			resultBone->AddChild(root);
		}

		return resultBone;
	}

   std::vector<AnimationSequence> AssimpSkeletonConverter::ConvertAssimpAnimationToEngineAnimation(const std::vector<AnimationLOADER>& srcAnimations) const
   {
      std::vector<AnimationSequence> dstAnimations;

      for (auto& srcAnimation : srcAnimations)
      {
         std::vector<AnimationFrame> dstFrames;
         for (auto& srcFrameCollection : srcAnimation.FramesBoneCollection)
         {
            AnimationFrame dstFrame(srcFrameCollection.BoneName);
            for (auto& srcFrame : srcFrameCollection.Frames)
            {
               dstFrame.AddFrame(BoneTransform(srcFrame.Rotation, srcFrame.Translation, srcFrame.Scale), srcFrame.TimeStart);
            }
            dstFrames.emplace_back(std::move(dstFrame));
         }
         dstAnimations.emplace_back(std::move(AnimationSequence(srcAnimation.Name, dstFrames, srcAnimation.AnimationDuration)));
      }

      return dstAnimations;
   }

}
