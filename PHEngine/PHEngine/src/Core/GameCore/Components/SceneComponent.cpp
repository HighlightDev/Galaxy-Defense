#include "SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Event/SceneComponentTransformChangedEvent.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineUtility;

namespace Game
{

   SceneComponent::SceneComponent()
      : Component()
      , bTransformationDirty(true, std::function<void(void)>([=](){ Event::SceneComponentTransformChangedEvent::GetInstance()->SendEvent(GetComponentType()); }))
      , m_translation(0)
      , m_rotation(0)
      , m_scale(1)
      , m_relativeMatrix(1)
      , m_scene(nullptr)
   {

   }

	SceneComponent::SceneComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
		: Component()
      , bTransformationDirty(true, std::function<void(void)>([=]() { Event::SceneComponentTransformChangedEvent::GetInstance()->SendEvent(GetComponentType()); }))
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

		const float pitchRad = DEG_TO_RAD(m_rotation.x);
		const float yawRad = DEG_TO_RAD(m_rotation.y);
		const float rollRad = DEG_TO_RAD(m_rotation.z);

		glm::mat4 worldMatrix(1), identityMatrix(1);
      glm::mat4 scaleMatrix = glm::scale(identityMatrix, m_scale);
      glm::mat4 rotationMatrixX = glm::rotate(identityMatrix, pitchRad, AXIS_RIGHT);
      glm::mat4 rotationMatrixY = glm::rotate(identityMatrix, yawRad, AXIS_UP);
      glm::mat4 rotationMatrixZ = glm::rotate(identityMatrix, rollRad, AXIS_FORWARD);
	   glm::mat4 translationMatrix = glm::translate(identityMatrix, m_translation);
      worldMatrix *= parentRelativeMatrix;
      worldMatrix *= scaleMatrix;
      worldMatrix *= rotationMatrixX;
      worldMatrix *= rotationMatrixY;
      worldMatrix *= rotationMatrixZ;
      worldMatrix *= translationMatrix;
     

		m_relativeMatrix = std::move(worldMatrix);

		bTransformationDirty = false;
	}

}
