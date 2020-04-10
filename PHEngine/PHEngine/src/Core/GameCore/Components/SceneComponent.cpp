#include "SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;

namespace Game
{

   SceneComponent::SceneComponent()
      : Component()
      , bTransformationDirty(true)
      , mIsVisible(true)
      , m_translation(0)
      , m_eulerRotationDegrees(0)
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
		, m_eulerRotationDegrees(rotation)
		, m_scale(scale)
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

   glm::mat3 SceneComponent::GetEuelerRotationMatrix() const
   {
      glm::mat4 identityMatrix(1);
      glm::mat4 resultMatrix = identityMatrix;

      if (!CompareFloats(m_eulerRotationDegrees.x, 0.0f))
      {
         const float pitchRad = DEG_TO_RAD(m_eulerRotationDegrees.x);
         resultMatrix *= glm::rotate(identityMatrix, pitchRad, AXIS_RIGHT);
      }

      if (!CompareFloats(m_eulerRotationDegrees.y, 0.0f))
      {
         const float yawRad = DEG_TO_RAD(m_eulerRotationDegrees.y);
         resultMatrix *= glm::rotate(identityMatrix, yawRad, AXIS_UP);
      }

      if (!CompareFloats(m_eulerRotationDegrees.z, 0.0f))
      {
         const float rollRad = DEG_TO_RAD(m_eulerRotationDegrees.z);
         resultMatrix *= glm::rotate(identityMatrix, rollRad, AXIS_FORWARD);
      }

      return resultMatrix;
   }

	void SceneComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
	{
		// Update current relative matrix

      glm::mat4 identityMatrix(1);
      m_relativeMatrix = identityMatrix;

	   glm::mat4 translationMatrix = glm::translate(identityMatrix, m_translation);

      m_relativeMatrix *= parentRelativeMatrix;
      m_relativeMatrix *= glm::scale(identityMatrix, m_scale);;
      m_relativeMatrix *= translationMatrix;

      if (bIsRootComponent)
      {
         glm::mat4 cameraYawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(m_additionalRotation.y), AXIS_UP);
         m_relativeMatrix *= cameraYawRotation;
      }

      if (!CompareFloats(m_eulerRotationDegrees.x, 0.0f))
      {
         const float pitchRad = DEG_TO_RAD(m_eulerRotationDegrees.x);
         m_relativeMatrix *= glm::rotate(identityMatrix, pitchRad, AXIS_RIGHT);
      }

      if (!CompareFloats(m_eulerRotationDegrees.y, 0.0f))
      {
         const float yawRad = DEG_TO_RAD(m_eulerRotationDegrees.y);
         m_relativeMatrix *= glm::rotate(identityMatrix, yawRad, AXIS_UP);
      }

      if (!CompareFloats(m_eulerRotationDegrees.z, 0.0f))
      {
         const float rollRad = DEG_TO_RAD(m_eulerRotationDegrees.z);
         m_relativeMatrix *= glm::rotate(identityMatrix, rollRad, AXIS_FORWARD);
      }

		SetIsTransformationDirty(false, true);
	}

}
