#pragma once

#include "LightSceneProxy.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GameCore/BoundingBox3D.h"

using namespace EngineCore;

namespace Graphics
{
   namespace Proxy
   {

      class DirectionalLightSceneProxy : public LightSceneProxy
      {
         glm::vec3 m_direction;

      public:
         glm::vec3 GetDirection() const;

         DirectionalLightSceneProxy(const DirectionalLightComponent *component);

         ~DirectionalLightSceneProxy() override;

         void PostInitialize() override;

         std::shared_ptr<ProjectedDirectionalLightShadowInfo> GetProjectedDirShadowInfo();

         LightSceneProxyType GetLightProxyType() const override;

         std::shared_ptr<ProjectedShadowInfo> GetShadowInfo() override;

         BoundingBox3D GetShadowOrthographicProjectionBound() const;
      };

   }
}
