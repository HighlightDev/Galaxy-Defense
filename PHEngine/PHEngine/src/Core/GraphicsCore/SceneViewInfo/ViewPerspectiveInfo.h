#pragma once

namespace Graphics
{
   struct ViewPerspectiveInfo
   {
      float FoV;
      float AspectRatio;
      float NearPlane;
      float FarPlane;

      ViewPerspectiveInfo()
         : FoV(0.0f)
         , AspectRatio(0.0f)
         , NearPlane(0.0f)
         , FarPlane(0.0f)
      {
      }

      ViewPerspectiveInfo(const float _FoV, const float _AspectRatio, const float _NearPlane, const float _FarPlane)
         : FoV(_FoV)
         , AspectRatio(_AspectRatio)
         , NearPlane(_NearPlane)
         , FarPlane(_FarPlane)
      {
      }
   };
}