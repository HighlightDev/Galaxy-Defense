#include "LightSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      LightSceneProxy::LightSceneProxy(const bool isEnabled,
                                       const glm::mat4 &relativeMatrix,
                                       const glm::vec3 &ambientColor,
                                       const glm::vec3 &diffuseColor,
                                       const glm::vec3 &specularColor,
                                       const std::shared_ptr<ProjectedShadowInfo> &shadowInfo)
          : SceneProxyBase(isEnabled),
            m_relativeMatrix(relativeMatrix),
            m_shadowInfo(shadowInfo),
            AmbientColor(ambientColor),
            DiffuseColor(diffuseColor),
            SpecularColor(specularColor)
      {
      }

      LightSceneProxy::~LightSceneProxy()
      {
      }

      void LightSceneProxy::CleanUp()
      {
         if (m_shadowInfo)
         {
            m_shadowInfo->CleanUp();
         }
      }

      void LightSceneProxy::PostLevelInit()
      {
      }

      void LightSceneProxy::SetTransformationMatrix(const glm::mat4 &relativeMatrix)
      {
         m_relativeMatrix = relativeMatrix;
         SetIsTransformationDirty(true);
      }

      bool LightSceneProxy::IsTransformationDirty() const
      {
         return bTransformationDirty;
      }

      void LightSceneProxy::SetIsTransformationDirty(bool value)
      {
         bTransformationDirty = value;
      }

      std::shared_ptr<ProjectedShadowInfo> LightSceneProxy::GetShadowInfo()
      {
         return m_shadowInfo;
      }

   }
}
