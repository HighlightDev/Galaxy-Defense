#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <stdint.h>

namespace EngineCore
{
    class Scene;
    namespace Scripts
    {
        class IEngineComponentCreatorFactory
        {
        public:
            virtual void CreateComponent(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                                         const int32_t actorObjectId,
                                         const std::string &componentType,
                                         const std::string &componentDataJsonStr) const = 0;
        };
    }
}
