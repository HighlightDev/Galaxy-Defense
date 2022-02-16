#include "SkyboxComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GraphicsCore/SceneProxy/SkyboxSceneProxy.h"
#include "Core/GameCore/Serialize/SerializeHelper.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/ComponentData/SkyboxComponentData.h"

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Game
{
	using namespace EngineUtility;

	SkyboxComponent::SkyboxComponent(const SkyboxComponentData& data, const SkyboxRenderData& renderData)
		: PrimitiveComponent(data.GameObjectName, glm::vec3(), glm::vec3(), data.m_scale, renderData.m_skin->GetBoundingBox())
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

   void SkyboxComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);

      std::shared_ptr<SerializeDataSkyboxComponent> skyboxCompData = std::make_shared<SerializeDataSkyboxComponent>();

      skyboxCompData->ComponentName = GameObjectName;
      skyboxCompData->Scale = GetScale();
      skyboxCompData->Material = SerializeHelper::GetSerializeDataMaterial(GetMaterial());
      actorData.ComponentsData.emplace_back(skyboxCompData);
   }

   std::shared_ptr<PrimitiveSceneProxy> SkyboxComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkyboxSceneProxy>(this);
   }

   ComponentType SkyboxComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void SkyboxComponent::SetRotateSpeed(float rotateSpeed)
   {
      m_rotateSpeed = rotateSpeed;
   }

   float SkyboxComponent::GetRotateSpeed() const
   {
      return m_rotateSpeed;
   }

   const SkyboxRenderData& SkyboxComponent::GetRenderData() const {

      return m_renderData;
   }

   std::shared_ptr<IMaterial> SkyboxComponent::GetMaterial() const
   {
      std::shared_ptr<IMaterial> materialResult = nullptr;
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
      }
      assert(materialResult != nullptr);
      return materialResult;
   }
}