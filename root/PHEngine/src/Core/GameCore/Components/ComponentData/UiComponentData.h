#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>

namespace EngineCore {

namespace GUI {
class UiCanvas;
}

struct UiComponentData : public ComponentData {
    std::shared_ptr<EngineCore::GUI::UiCanvas> Canvas;

    UiComponentData(const std::string& gameObjectName, const std::shared_ptr<EngineCore::GUI::UiCanvas>& canvas)
        : ComponentData(gameObjectName)
        , Canvas(canvas)
    {
        ext_assert(canvas != nullptr, "UiComponentData::UiComponentData: canvas is null");
    }

    virtual ~UiComponentData()
    {
    }
};

} // namespace EngineCore