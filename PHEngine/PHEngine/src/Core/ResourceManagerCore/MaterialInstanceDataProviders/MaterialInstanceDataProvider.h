#pragma once

#include <memory>

namespace EngineCore
{
    class Scene;
}

namespace Resources
{
    class IMaterialInstanceDataProvider
    {
    public:
        virtual ~IMaterialInstanceDataProvider() {}

        virtual std::weak_ptr<::EngineCore::Scene> GetSceneWp() const = 0;

        virtual int32_t GetInstanceObjectId() const = 0;

        // this id is identical to draw call instance 
        virtual int32_t GetInstanceId() const = 0;
    };
}
