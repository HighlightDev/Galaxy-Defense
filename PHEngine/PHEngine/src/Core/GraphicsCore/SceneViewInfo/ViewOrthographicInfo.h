#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ViewProjectionInfo.h"

namespace Graphics
{
   struct ViewOrthographicInfo
       : public ViewProjectionInfo
   {
   private:
      float mLeft{0.0f};
      float mRight{0.0f};
      float mBottom{0.0f};
      float mTop{0.0f};
      float mZNear{0.0f};
      float mZFar{0.0f};

   public:
      ViewOrthographicInfo(const float left,
                           const float right,
                           const float bottom,
                           const float top,
                           const float zNear,
                           const float zFar)
          : mLeft(left),
            mRight(right),
            mBottom(bottom),
            mTop(top),
            mZNear(zNear),
            mZFar(zFar)
      {
      }

      virtual glm::mat4 CreateProjectionMatrix() const
      {
         return glm::ortho<float>(mLeft, mRight, mBottom, mTop, mZNear, mZFar);
      }
   };
}