#pragma once

#include "RuntimeGeneratedLineSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

using namespace EngineCore;

namespace Graphics
{
   namespace Proxy
   {
      class RuntimeGeneratedQuadraticBezierCurveSceneProxy : public RuntimeGeneratedLineSceneProxy
      {
         using Base = RuntimeGeneratedLineSceneProxy;

         glm::vec3 mBezierControlPointWorldSpacePosition;

         int32_t mCurveSegmentsCount;

      public:
         RuntimeGeneratedQuadraticBezierCurveSceneProxy(const RuntimeGeneratedQuadraticBezierCurveComponent *component);

         void SetBezierControlPointWorldSpacePosition(const glm::vec3 &position);

         void SetCurveSegmentsCount(const int32_t curveSegmentsCount);

      protected:
         void UpdateGeometry(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix) override;
      };

   }
}
