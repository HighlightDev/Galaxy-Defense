#pragma once

#include <vector>
#include <stdint.h>
#include <assimp/mesh.h>

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{
         struct BoneInfo
         {
            aiString mName;

            uint32_t mNumWeights;

            aiMatrix4x4 mOffsetMatrix;
         };

			class SkeletonBoneLOADER;

			class SkeletonBoneBaseLOADER
			{
			protected:
				std::vector<SkeletonBoneLOADER*> m_children;
			public:

            aiMatrix4x4 InvGlobaTransform;

				SkeletonBoneBaseLOADER();

				virtual ~SkeletonBoneBaseLOADER();

				void AddChildBone(SkeletonBoneLOADER* child);

				inline std::vector<SkeletonBoneLOADER*>& GetChildren();

				int32_t GetIdByBoneInHierarchy(aiBone* seekBone) const;

				int32_t GetIdByBone(aiBone* seekBone, SkeletonBoneLOADER* currentSkeletonBone) const;

            void CleanUp();
			};

			class SkeletonBoneLOADER : public SkeletonBoneBaseLOADER
			{
			private:

				SkeletonBoneBaseLOADER* m_parent;
            BoneInfo m_boneInfo;
				int32_t m_boneId = -1;

			public:
				SkeletonBoneLOADER(SkeletonBoneBaseLOADER* parent);
				~SkeletonBoneLOADER();

				void SetBoneInfo(BoneInfo bone);

				void SetBoneId(int32_t id);

				inline BoneInfo GetBoneInfo() const;

				inline int32_t GetBoneId() const;

				inline SkeletonBoneBaseLOADER* GetParent() const;

				void CleanUp();
			};

		}
	}
}
