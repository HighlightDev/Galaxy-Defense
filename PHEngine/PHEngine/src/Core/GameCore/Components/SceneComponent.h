#pragma once
#include "Component.h"
#include "Core/CommonApi/VariableWrapper.h"
#include "Core/GameCore/Event/SceneComponentTransformChangedEvent.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/detail/qualifier.hpp>

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

      bool mIsVisible;

		glm::vec3 m_translation;
		glm::vec3 m_eulerRotationDegrees;
		glm::vec3 m_scale;

    

		glm::mat4 m_relativeMatrix;

      class Scene* m_scene;

      // ptr because this rotation is 
      glm::vec3 m_additionalRotation;

   public:

      struct glm::qua<float, glm::qualifier::packed_highp>* mRotator;

      bool bIsRootComponent = false;

		SceneComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

      SceneComponent();

		~SceneComponent();

		virtual void Tick(const float deltaTime) override;

      virtual uint64_t GetComponentType() const override;

		/* This method works every time when this component has dirty transform */
		virtual void UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix);

      void SetIsVisible(bool isVisible);

      inline bool IsVisible() const {
         return mIsVisible;
      }

      virtual void OnVisibilityChanged() {}

      inline void SetScene(Scene* scene)
      {
         m_scene = scene;
      }

      void SetIsTransformationDirty(const bool isDirty, const bool bTriggerTransformUpdateEvent = true)
      {
         bTransformationDirty = true;

         if (bTriggerTransformUpdateEvent)
            Event::SceneComponentTransformChangedEvent::GetInstance()->SendEvent(GetComponentType());
      }

		void SetTranslation(glm::vec3 translation, const bool bTriggerTransformUpdateEvent = true)
		{
			m_translation = translation;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

		void SetEulerRotationDegrees(glm::vec3 rotation, const bool bTriggerTransformUpdateEvent = true)
		{
			m_eulerRotationDegrees = rotation;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

		void SetScale(glm::vec3 scale, const bool bTriggerTransformUpdateEvent = true)
		{
			m_scale = scale;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

      void SetAdditionalRotation(const glm::vec3& rotation, const bool bTriggerTransformUpdateEvent = true)
      {
         m_additionalRotation = rotation;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
      }

		void SetRotationAxisX(float new_x, const bool bTriggerTransformUpdateEvent = true)
		{
			if (new_x > 360.0f)
			{
				m_eulerRotationDegrees.x = new_x - 360.0f;
			}
			else
				m_eulerRotationDegrees.x = new_x;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

		void SetRotationAxisY(float new_y, const bool bTriggerTransformUpdateEvent = true)
		{
			if (new_y > 360.0f)
			{
				m_eulerRotationDegrees.y = new_y - 360.0f;
			}
			else
				m_eulerRotationDegrees.y = new_y;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

		void SetRotationAxisZ(float new_z, const bool bTriggerTransformUpdateEvent = true)
		{
			if (new_z > 360.0f)
			{
				m_eulerRotationDegrees.z = new_z - 360.0f;
			}
			else
				m_eulerRotationDegrees.z = new_z;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

      glm::mat3 GetEuelerRotationMatrix() const;

		inline bool GetIsTransformationDirty() const {
			return bTransformationDirty;
		}

		inline glm::vec3 GetTranslation() const
		{
			return m_translation;
		}

		inline glm::vec3 GetEulerRotationDegrees() const
		{
			return m_eulerRotationDegrees;
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



