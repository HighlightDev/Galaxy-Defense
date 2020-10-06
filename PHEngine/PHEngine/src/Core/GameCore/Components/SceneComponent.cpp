#include "SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace EngineMath;

namespace Game
{

   SceneComponent::SceneComponent(const std::string& gameObjectName)
      : Component(gameObjectName)
      , bTransformationDirty(true)
      , mIsVisible(GenericObjectProperty<bool>(true, "IsVisible"))
      , mTransform(std::make_shared<Transform>())
      , m_additionalRotationEuler()
      , m_relativeMatrix(1)
      , m_scene(nullptr)
   {
      /******  HOOKS ****/
      ENGINE_PROPERTY("IsVisible", &mIsVisible);
      /******  HOOKS ****/
   }

	SceneComponent::SceneComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
		: Component(gameObjectName)
      , bTransformationDirty(true)
      , mIsVisible(GenericObjectProperty<bool>(true, "IsVisible"))
      , mTransform(std::make_shared<Transform>(translation, glm::quat(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z))), scale))
      , m_additionalRotationEuler()
		, m_relativeMatrix(std::move(glm::mat4(1)))
      , m_scene(nullptr)
	{
      /******  HOOKS ****/
      ENGINE_PROPERTY("IsVisible", &mIsVisible);
      /******  HOOKS ****/
	}

	SceneComponent::~SceneComponent()
	{
	}

	void SceneComponent::Tick(const float deltaTime)
	{
	}

   uint64_t SceneComponent::GetComponentType() const
   {
      return SCENE_COMPONENT;
   }

   void SceneComponent::SetIsVisible(bool isVisible)
   {
      if (isVisible != mIsVisible)
      {
         mIsVisible = isVisible;
         OnVisibilityChanged();
      }
   }
 
	void SceneComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
	{
		// Update current relative matrix

      glm::mat4 identityMatrix(1);
      m_relativeMatrix = identityMatrix;

	   glm::mat4 translationMatrix = glm::translate(identityMatrix, mTransform->Translation);

      m_relativeMatrix *= parentRelativeMatrix;
      m_relativeMatrix *= glm::scale(identityMatrix, mTransform->Scale);
      m_relativeMatrix *= translationMatrix;

      if (bIsRootComponent)
      {
         glm::mat4 cameraYawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(m_additionalRotationEuler.y), AXIS_UP);
         m_relativeMatrix *= cameraYawRotation;     
      }

      glm::mat4 rotationMatrix = glm::toMat4(mTransform->Rotator);
      m_relativeMatrix *= rotationMatrix;

		SetIsTransformationDirty(false);
	}

}
