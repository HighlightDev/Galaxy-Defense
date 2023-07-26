#pragma once

#include "LightSceneProxy.h"
#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"

using namespace EngineCore;

namespace Graphics
{
   namespace Proxy
   {

      class PointLightSceneProxy : public LightSceneProxy
      {
      protected:
         glm::vec3 m_attenuation;

         float m_radianceRadius;

      public:
         PointLightSceneProxy(const PointLightComponent *component);

         ~PointLightSceneProxy() override;

         void PostLevelInit() override;

         glm::vec3 GetPosition() const;

         glm::vec3 GetAttenuation() const;

         float GetRadianceRadius() const;

         LightSceneProxyType GetLightProxyType() const override;

         std::shared_ptr<ProjectedPointLightShadowInfo> GetProjectedPointShadowInfo();

         std::shared_ptr<ProjectedShadowInfo> GetShadowInfo() override;
      };

   }
}
