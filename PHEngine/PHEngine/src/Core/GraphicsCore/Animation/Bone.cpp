#include "Bone.h"

namespace Graphics
{
	namespace Animation
	{

		Bone::Bone(int32_t boneId, std::string boneName, Bone* parentBone)
			: m_parent(parentBone)
			, m_id(boneId)
			, m_name(boneName)
			, m_offsetMatrix(std::move(glm::mat4()))
		{
		}

		Bone::~Bone()
		{
		}
	
		void Bone::CleanUp() {

			ClearChildren();
		}

		std::vector<glm::mat4> Bone::GetChildrenOffsetMatrices() const {

			std::vector<glm::mat4> resultOffsetMatrices;

			for (auto& child : m_children)
			{
				resultOffsetMatrices.emplace_back(std::move(child->GetOffsetMatrix()));
				IterateOffsetHierarchy(child, resultOffsetMatrices);
			}

			return resultOffsetMatrices;
		}

		void Bone::IterateOffsetHierarchy(Bone* parentBone, std::vector<glm::mat4>& out_offsetMatrices) const {

			for (auto& child : parentBone->m_children)
			{
				out_offsetMatrices.emplace_back(std::move(child->GetOffsetMatrix()));
				IterateOffsetHierarchy(child, out_offsetMatrices);
			}
		}


	}
}
