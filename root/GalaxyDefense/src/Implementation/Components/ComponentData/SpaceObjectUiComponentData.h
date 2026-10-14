#pragma once

#include "Core/GameCore/Components/ComponentData/UiComponentData.h"

#include <string>

using namespace EngineCore;

namespace EngineCore::GUI {
class UiCanvas;
}

namespace EngineCore {
class SceneComponent;
}

namespace Game {
struct SpaceObjectUiComponentData : public UiComponentData {

    std::weak_ptr<::EngineCore::SceneComponent> mOwnerRootComponent;

    SpaceObjectUiComponentData(
        const std::string& gameObjectName,
        const std::shared_ptr<::EngineCore::GUI::UiCanvas>& canvas,
        const std::weak_ptr<::EngineCore::SceneComponent>& ownerRootComponent)
        : UiComponentData(gameObjectName, canvas)
        , mOwnerRootComponent(ownerRootComponent)
    {
    }
};
} // namespace Game