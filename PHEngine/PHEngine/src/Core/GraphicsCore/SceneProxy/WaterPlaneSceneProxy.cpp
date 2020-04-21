#include "WaterPlaneSceneProxy.h"

#include <glm/vec3.hpp>

namespace Graphics
{
   namespace Proxy
   {

      WaterPlaneSceneProxy::WaterPlaneSceneProxy(const WaterPlaneComponent* component)
         : PrimitiveSceneProxy(component->GetRelativeMatrix(), component->GetRenderData().m_skin, component->GetRenderData().m_materialShader, component->GetRenderData().mMaterialInstance)
         , m_moveFactor(component->GetMoveFactor())
         , m_waveStrength(component->GetWaveStrength())
         , m_transparencyDepth(component->GetTransparencyDepth())
         , m_nearClipPlane(component->GetNearClipPlane())
         , m_farClipPlane(component->GetFarClipPlane())
      {
         Init();
      }

      std::shared_ptr<WaterPlaneSceneProxy::ShaderType> WaterPlaneSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<WaterPlaneSceneProxy::ShaderType>(m_shader);
      }

      WaterPlaneSceneProxy::~WaterPlaneSceneProxy()
      {
      }

      void WaterPlaneSceneProxy::Init()
      {
         // m_waterPlaneFramebuffer = std::make_unique<WaterPlaneFramebuffer>();
      }

      void WaterPlaneSceneProxy::SetMoveFactor(float moveFactor) 
      {
         m_moveFactor = moveFactor;
         std::static_pointer_cast<WaterPlaneSceneProxy::MaterialType>(mMaterialInstance)->SetMoveFactor(moveFactor);
      }

      void WaterPlaneSceneProxy::SetWaveStrength(float waveStr)
      {
         m_waveStrength = waveStr;
         std::static_pointer_cast<WaterPlaneSceneProxy::MaterialType>(mMaterialInstance)->SetMoveStrengthFactor(waveStr);
      }

      void WaterPlaneSceneProxy::SetTransparencyDepth(float transparencyDepth)
      {
         m_transparencyDepth = transparencyDepth;
      }

      void WaterPlaneSceneProxy::SetNearClipPlane(float nearClipPlane)
      {
         m_nearClipPlane = nearClipPlane;
      }

      void WaterPlaneSceneProxy::SetFarClipPlane(float farClipPlane)
      {
         m_farClipPlane = farClipPlane;
      }

      uint64_t WaterPlaneSceneProxy::GetComponentType() const
      {
         return WATER_PLANE_COMPONENT;
      }

      bool WaterPlaneSceneProxy::IsDeferred() const
      {
         return false;
      }

      void WaterPlaneSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         //glm::vec3 cameraPosition(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2]);

         const auto& shader = GetShader();

         shader->ExecuteShader();
         shader->GetMaterialShader()->SetUniformValues(mMaterialInstance);
         shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shader->StopShader();

         //m_shader->ExecuteShader();
         //m_waterPlaneFramebuffer->ReflectionTexture->BindTexture(0);
         //m_waterPlaneFramebuffer->RefractionTexture->BindTexture(1);
         //m_waterDistortionMap->BindTexture(2);
         //m_waterNormalMap->BindTexture(3);
         //m_waterPlaneFramebuffer->DepthTexture->BindTexture(4);
         //m_shader->SetTransformationMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         //m_shader->SetReflectionSampler(0);
         //m_shader->SetRefractionSampler(1);
         //m_shader->SetDuDvSampler(2);
         //m_shader->SetNormalMapSampler(3);
         //m_shader->SetDepthSampler(4);
         //m_shader->SetCameraPosition(cameraPosition);
         //m_shader->SetDistortionProperties(m_moveFactor, m_waveStrength);
         ////m_shader->SetDirectionalLight(sun);
         ////m_shader->SetPointLight(lights);
         //m_shader->SetClippingPlanes(m_nearClipPlane, m_farClipPlane);
         ////m_shader->SetMist(m_mist);
         //m_shader->SetTransparancyDepth(m_transparencyDepth);

         /*glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         glDisable(GL_BLEND);
         m_shader->StopShader();*/
      }
   }
}
