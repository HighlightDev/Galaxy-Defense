#pragma once

#include <memory>
#include <string>

namespace EngineCore {
class Scene;
}

namespace Resources {
class MaterialInstanceDataProvider {
protected:
    // this id is identical to draw call instance
    int32_t mRenderInstanceId{-1};

    bool mIsInstanceActive{false};

public:
    virtual ~MaterialInstanceDataProvider()
    {
    }

    virtual std::weak_ptr<::EngineCore::Scene> GetSceneWp() const = 0;

    virtual int32_t GetInstanceObjectId() const = 0;

    virtual int32_t GetInstanceProxyId() const = 0;

    // this id is identical to draw call instance
    int32_t GetRenderInstanceId() const;

    bool IsInstanceActive() const;
};
} // namespace Resources
