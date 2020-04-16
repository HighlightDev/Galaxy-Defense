#include "SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace EngineMath;

namespace Game
{

   SceneComponent::SceneComponent()
      : Component()
      , bTransformationDirty(true)
      , mIsVisible(true)
      , m_translation(0)
      , mRotator(glm::vec3(0))
      , m_scale(1)
      , m_relativeMatrix(1)
      , m_scene(nullptr)
   {

   }

	SceneComponent::SceneComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
		: Component()
      , bTransformationDirty(true)
      , mIsVisible(true)
		, m_translation(translation)
		, m_scale(scale)
      , mRotator(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z)))
		, m_relativeMatrix(std::move(glm::mat4(1)))
      , m_scene(nullptr)
	{
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
         OnVisibilityChanged();
         mIsVisible = isVisible;
      }
   }
 
	void SceneComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
	{
		// Update current relative matrix

      glm::mat4 identityMatrix(1);
      m_relativeMatrix = identityMatrix;

	   glm::mat4 translationMatrix = glm::translate(identityMatrix, m_translation);

      m_relativeMatrix *= parentRelativeMatrix;
      m_relativeMatrix *= glm::scale(identityMatrix, m_scale);
      m_relativeMatrix *= translationMatrix;

      if (bIsRootComponent)
      {
         glm::mat4 cameraYawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(m_additionalRotationEuler.y), AXIS_UP);
         m_relativeMatrix *= cameraYawRotation;     
      }

      glm::mat4 rotationMatrix = glm::toMat4(mRotator);
      m_relativeMatrix *= rotationMatrix;

		SetIsTransformationDirty(false, true);
	}

}
