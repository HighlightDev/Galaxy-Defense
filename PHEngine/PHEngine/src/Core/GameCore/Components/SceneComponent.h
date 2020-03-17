#pragma once
#include "Component.h"
#include "Core/CommonApi/VariableWrapper.h"
#include "Core/GameCore/Event/SceneComponentTransformChangedEvent.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Game
{
	class SceneComponent 
      : public Component
	{
   public:

      using wrapped_bool = VariableWrapper<bool>;
      
   protected:

		using Base = Component;

      bool bTransformationDirty;

		glm::vec3 m_translation;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;

		glm::mat4 m_relativeMatrix;

      class Scene* m_scene;

      // ptr because this rotation is 
      glm::vec3 m_additionalRotation;

   public:

      bool bIsRootComponent = false;

      glm::vec3 phys_translation;

		SceneComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

      SceneComponent();

		~SceneComponent();

		virtual void Tick(const float deltaTime) override;

      virtual uint64_t GetComponentType() const override;

		/* This method works every time when this component has dirty transform */
		virtual void UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix);

      inline void SetScene(Scene* scene)
      {
         m_scene = scene;
      }

      void SetIsTransformationDirty(const bool isDirty)
      {
         bTransformationDirty = true;
         Event::SceneComponentTransformChangedEvent::GetInstance()->SendEvent(GetComponentType());
      }

		void SetTranslation(glm::vec3 translation)
		{
			m_translation = translation;
         SetIsTransformationDirty(true);
		}

		void SetRotation(glm::vec3 rotation)
		{
			m_rotation = rotation;
         SetIsTransformationDirty(true);
		}

		void SetScale(glm::vec3 scale)
		{
			m_scale = scale;
         SetIsTransformationDirty(true);
		}

      void SetAdditionalRotation(const glm::vec3& rotation)
      {
         m_additionalRotation = rotation;
         SetIsTransformationDirty(true);
      }

		void SetRotationAxisX(float new_x)
		{
			if (new_x > 360.0f)
			{
				m_rotation.x = new_x - 360.0f;
			}
			else
				m_rotation.x = new_x;
         SetIsTransformationDirty(true);
		}

		void SetRotationAxisY(float new_y)
		{
			if (new_y > 360.0f)
			{
				m_rotation.y = new_y - 360.0f;
			}
			else
				m_rotation.y = new_y;
         SetIsTransformationDirty(true);
		}

		void SetRotationAxisZ(float new_z)
		{
			if (new_z > 360.0f)
			{
				m_rotation.z = new_z - 360.0f;
			}
			else
				m_rotation.z = new_z;
         SetIsTransformationDirty(true);
		}

		inline bool GetIsTransformationDirty() const {
			return bTransformationDirty;
		}

		inline glm::vec3 GetTranslation() const
		{
			return m_translation;
		}

		inline glm::vec3 GetRotation() const
		{
			return m_rotation;
		}

		inline glm::vec3 GetScale() const
		{
			return m_scale;
		}

		inline glm::mat4 GetRelativeMatrix() const
		{
			return m_relativeMatrix;
		}
	};
}



