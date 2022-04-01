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
                                       ProjectedShadowInfo *shadowInfo)
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
         delete m_shadowInfo;
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

      ProjectedShadowInfo *LightSceneProxy::GetShadowInfo()
      {

         return m_shadowInfo;
      }

   }
}
