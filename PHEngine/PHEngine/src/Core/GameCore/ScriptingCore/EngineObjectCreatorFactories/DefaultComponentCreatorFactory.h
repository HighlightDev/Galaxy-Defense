#pragma once

#include "IEngineComponentCreatorFactory.h"

#include <memory>

namespace EngineCore
{
    class Scene;
    struct ComponentData;

    namespace Scripts
    {
        class DefaultComponentCreatorFactory
            : public IEngineComponentCreatorFactory
        {
        public:
            void CreateComponent(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                                 const int32_t actorObjectId,
                                 const std::string &componentType,
                                 const std::string &componentDataJsonStr) const override;

        private:
            std::shared_ptr<ComponentData> CreateComponentData(const std::shared_ptr<::EngineCore::Scene> &sceneSp,
                                                               const std::string &componentType,
                                                               const std::string &componentDataJsonStr) const;
        };
    }
}
