#include "SkeletonBoneLOADER.h"

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			/********* SkeletonBoneBaseLOADER ***************/
			SkeletonBoneBaseLOADER::SkeletonBoneBaseLOADER()
			{

			}

			SkeletonBoneBaseLOADER::~SkeletonBoneBaseLOADER()
			{
			}

			void SkeletonBoneBaseLOADER::AddChildBone(SkeletonBoneLOADER* child)
			{
				m_children.push_back(child);
			}

			std::vector<SkeletonBoneLOADER*>& SkeletonBoneBaseLOADER::GetChildren()
			{
				return m_children;
			}

			int32_t SkeletonBoneBaseLOADER::GetIdByBoneInHierarchy(aiBone* seekBone) const
			{
				int32_t id = -1;

				for (auto& child : m_children)
				{
					id = child->GetIdByBone(seekBone, child);
					if (id >= 0)
						break;
				}

				return id;
			}

			int32_t SkeletonBoneBaseLOADER::GetIdByBone(aiBone* seekBone, SkeletonBoneLOADER* currentSkeletonBone) const
			{
				int32_t id = -1;

				if (currentSkeletonBone->GetBoneInfo()->mName == seekBone->mName)
				{
					id = currentSkeletonBone->GetBoneId();
				}

				if (id < 0)
				{
					for (auto& skeletonBone : currentSkeletonBone->GetChildren())
					{
						id = skeletonBone->GetIdByBone(seekBone, skeletonBone);
						if (id > 0)
							return id;
					}
				}

				return id;
			}

         void SkeletonBoneBaseLOADER::CleanUp()
         {
            for (size_t index = 0; index < m_children.size(); index++)
            {
               SkeletonBoneLOADER* child = m_children[index];
               child->CleanUp();
               delete child;
            }
            m_children.clear();
         }

			/********* SkeletonBoneLOADER ***************/

			SkeletonBoneLOADER::SkeletonBoneLOADER(SkeletonBoneBaseLOADER* parent)
				: m_parent(parent)
			{
			}


			SkeletonBoneLOADER::~SkeletonBoneLOADER()
			{
			}

			void SkeletonBoneLOADER::SetBoneInfo(aiBone* bone)
			{
				m_boneInfo = bone;
			}

			void SkeletonBoneLOADER::SetBoneId(int32_t id)
			{
				m_boneId = id;
			}

			aiBone* SkeletonBoneLOADER::GetBoneInfo() const
			{
				return m_boneInfo;
			}

			int32_t SkeletonBoneLOADER::GetBoneId() const
			{
				return m_boneId;
			}

			SkeletonBoneBaseLOADER* SkeletonBoneLOADER::GetParent() const
			{
				return m_parent;
			}

			void SkeletonBoneLOADER::CleanUp()
			{
				for (size_t index = 0; index < m_children.size(); index ++)
				{
               SkeletonBoneLOADER* child = m_children[index];
					child->CleanUp();
               delete child;
				}
				m_children.clear();
			}

		}
	}
}