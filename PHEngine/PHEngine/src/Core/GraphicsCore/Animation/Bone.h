#pragma once

#include <vector>
#include <glm/mat4x4.hpp>
#include <stdint.h>
#include <string>
#include <utility>
#include <algorithm>

namespace Graphics
{
	namespace Animation
	{

		class Bone
		{
		protected:

			std::vector<Bone*> m_children;

			Bone* m_parent;

			int32_t m_id;

			std::string m_name;

			glm::mat4 m_offsetMatrix;

		public:

			Bone(int32_t boneId, std::string boneName, Bone* parentBone = nullptr);

			~Bone();

			inline void AddChild(Bone* child) {

				m_children.push_back(child);
			}

			inline void RemoveChild(Bone* child) {

				auto it = std::find_if(m_children.begin(), m_children.end(), [&](Bone*& bone) { return child->GetId() == bone->GetId(); });
				m_children.erase(it);
			}

			inline void ClearChildren() {

				m_children.clear();
			}

			void CleanUp();

			std::vector<glm::mat4> GetChildrenOffsetMatrices() const;

			void IterateOffsetHierarchy(Bone* parentBone, std::vector<glm::mat4>& out_offsetMatrices) const;

			inline std::vector<Bone*>& GetChildren() {

				return m_children;
			}

			inline Bone* GetParent() const
			{
				return m_parent;
			}

			inline int32_t GetId() const {

				return m_id;
			}

			inline std::string GetName() const {

				return m_name;
			}

			inline glm::mat4 GetOffsetMatrix() const {

				return m_offsetMatrix;
			}

			inline void SetOffsetMatrix(glm::mat4& offsetMatrix) {

				m_offsetMatrix = offsetMatrix;
			}

			inline void SetOffsetMatrix(glm::mat4&& offsetMatrix) {

				m_offsetMatrix = std::move(offsetMatrix);
			}

		};

	}
}

