#pragma once
#include "Component.h"
#include "Core/CommonApi/VariableWrapper.h"
#include "Core/GameCore/Event/SceneComponentTransformChangedEvent.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/quaternion_float.hpp>

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
      glm::quat mRotator;
		glm::vec3 m_scale;

		glm::mat4 m_relativeMatrix;

      class Scene* m_scene;

      glm::vec3 m_additionalRotationEuler; // TODO: move to quat

   public:

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

		void SetTranslation(const glm::vec3& translation, const bool bTriggerTransformUpdateEvent = true)
		{
			m_translation = translation;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

		void SetRotator(const glm::quat& rotator, const bool bTriggerTransformUpdateEvent = true)
		{
			mRotator = rotator;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

		void SetScale(glm::vec3 scale, const bool bTriggerTransformUpdateEvent = true)
		{
			m_scale = scale;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
		}

      void SetAdditionalRotation(const glm::vec3& rotationEuler, const bool bTriggerTransformUpdateEvent = true)
      {
         m_additionalRotationEuler = rotationEuler;
         SetIsTransformationDirty(true, bTriggerTransformUpdateEvent);
      }

		inline bool GetIsTransformationDirty() const {
			return bTransformationDirty;
		}

		inline glm::vec3 GetTranslation() const
		{
			return m_translation;
		}

		inline glm::quat GetRotator() const
		{
			return mRotator;
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



