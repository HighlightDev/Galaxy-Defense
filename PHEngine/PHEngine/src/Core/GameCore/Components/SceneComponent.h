#pragma once
#include "Component.h"
#include "Core/CommonCore/VariableWrapper.h"
#include "Core/GameCore/Components/Transform.h"

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

      bool mIsVisible;

      std::shared_ptr<Transform> mTransform;

      glm::vec3 m_additionalRotationEuler; // TODO: move to quat

		glm::mat4 m_relativeMatrix;

      class Scene* m_scene;

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

      inline void SetScene(class Scene* scene)
      {
         m_scene = scene;
      }

      void SetIsTransformationDirty(const bool isDirty)
      {
         bTransformationDirty = true;
      }

		void SetTranslation(const glm::vec3& translation)
		{
         mTransform->Translation = translation;
         SetIsTransformationDirty(true);
		}

		void SetRotator(const glm::quat& rotator)
		{
         mTransform->Rotator = rotator;
         SetIsTransformationDirty(true);
		}

		void SetScale(glm::vec3 scale, const bool bTriggerTransformUpdateEvent = true)
		{
         mTransform->Scale = scale;
         SetIsTransformationDirty(true);
		}

      void SetAdditionalRotation(const glm::vec3& rotationEuler, const bool bTriggerTransformUpdateEvent = true)
      {
         m_additionalRotationEuler = rotationEuler;
         SetIsTransformationDirty(true);
      }

      std::weak_ptr<Transform> GetTransformWeakPtr() const {
         return mTransform;
      }

		inline bool GetIsTransformationDirty() const {
			return bTransformationDirty;
		}

		inline glm::vec3 GetTranslation() const
		{
			return mTransform->Translation;
		}

		inline glm::quat GetRotator() const
		{
			return mTransform->Rotator;
		}

		inline glm::vec3 GetScale() const
		{
			return mTransform->Scale;
		}

		inline glm::mat4 GetRelativeMatrix() const
		{
			return m_relativeMatrix;
		}
	};
}



