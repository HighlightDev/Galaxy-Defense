#pragma once

#include "Core/GameCore/BoundingBox3D.h"

using namespace EngineCore;

namespace Graphics {

class AProxyVisibilityController {
protected:
    BoundingBox3D mBoundingBox;

    bool mIsVisible;

public:
    AProxyVisibilityController(const bool bVisible);

    BoundingBox3D GetTransformedBoundingBox() const;

    void SetTransformedBoundingBox(const BoundingBox3D& boundingBox);

    // Method returns false when no frustum call is needed for primitive
    virtual bool IsFrustumCullTestNeeded() const = 0;

    void SetVisibility(const bool visibility);

    bool IsVisible() const;
};

} // namespace Graphics
