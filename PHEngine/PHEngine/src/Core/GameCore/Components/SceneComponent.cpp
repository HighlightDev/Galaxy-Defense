#include "SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineUtility;

namespace Game
{

   SceneComponent::SceneComponent()
      : Component()
      , bTransformationDirty(true)
      , m_translation(0)
      , m_rotation(0)
      , m_scale(1)
      , m_relativeMatrix(1)
      , m_scene(nullptr)
   {

   }

	SceneComponent::SceneComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
		: Component()
      , bTransformationDirty(true)
		, m_translation(translation)
		, m_rotation(rotation)
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
		Base::Tick(deltaTime);
	
	}

   uint64_t SceneComponent::GetComponentType() const
   {
      return SCENE_COMPONENT;
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

      if (!EngineUtility::CMP::Process(m_rotation.x, 0.0f))
      {
         const float pitchRad = DEG_TO_RAD(m_rotation.x);
         m_relativeMatrix *= glm::rotate(identityMatrix, pitchRad, AXIS_RIGHT);
      }

      if (!EngineUtility::CMP::Process(m_rotation.y, 0.0f))
      {
         const float yawRad = DEG_TO_RAD(m_rotation.y);
         m_relativeMatrix *= glm::rotate(identityMatrix, yawRad, AXIS_UP);
      }

      if (!EngineUtility::CMP::Process(m_rotation.z, 0.0f))
      {
         const float rollRad = DEG_TO_RAD(m_rotation.z);
         m_relativeMatrix *= glm::rotate(identityMatrix, rollRad, AXIS_FORWARD);
      }

		bTransformationDirty = false;
      Event::SceneComponentTransformChangedEvent::GetInstance()->SendEvent(GetComponentType());
	}

}
