#pragma once

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <array>

namespace Game
{

   class CameraFrustum
   {

      std::array<glm::vec4, 6> mPlanes;

   public:
      CameraFrustum();
      ~CameraFrustum();

      void ConstructFromViewProjectionMatrix(const glm::mat4& viewMatrix, const glm::mat4& projectioMatrix);
   };

}
