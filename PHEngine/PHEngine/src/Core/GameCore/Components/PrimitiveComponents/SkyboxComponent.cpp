#include "SkyboxComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GraphicsCore/SceneProxy/SkyboxSceneProxy.h"

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Game
{
	using namespace EngineUtility;

	SkyboxComponent::SkyboxComponent(const std::string& gameObjectName, const glm::vec3& scale, const SkyboxRenderData& renderData)
		: PrimitiveComponent(gameObjectName, glm::vec3(), glm::vec3(), scale)
		, m_rotateSpeed(2.0f)
      , m_renderData(renderData)
	{

	}

	SkyboxComponent::~SkyboxComponent()
	{

	}

	void SkyboxComponent::Tick(const float deltaTime)
	{
		Base::Tick(deltaTime);
		
      SetRotator(mTransform->Rotator * glm::angleAxis(DEG_TO_RAD(deltaTime * m_rotateSpeed), AXIS_UP));
	}

   std::shared_ptr<PrimitiveSceneProxy> SkyboxComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkyboxSceneProxy>(this);
   }

   ComponentType SkyboxComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }
}