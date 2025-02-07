#pragma once

#include "ViewProjectionInfo.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

namespace Graphics {
struct ViewPerspectiveInfo : public ViewProjectionInfo {
private:
    float FoV;
    float AspectRatio;
    float NearPlane;
    float FarPlane;

public:
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

    virtual glm::mat4 CreateProjectionMatrix() const
    {
        return glm::perspective<float>(FoV, AspectRatio, NearPlane, FarPlane);
    }
};
} // namespace Graphics