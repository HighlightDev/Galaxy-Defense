#pragma once

#include "Core/GameCore/ITickable.h"
#include "UiCanvas.h"

#include <glm/vec2.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
class Scene;
class UiInputComponent;

namespace GUI {
class UiHandler : public ITickable {
    std::weak_ptr<Scene> mOwner;

    std::vector<std::shared_ptr<UiCanvas>> mUiCanvases;

    std::shared_ptr<UiCanvas> mHudCanvas;

#ifdef DEBUG
    std::shared_ptr<UiCanvas> mDebugUiCanvas;

    std::unique_ptr<UiInputComponent> mInputComponent;
#endif

public:
    UiHandler();

    ~UiHandler();

    void SetScene(const std::weak_ptr<::EngineCore::Scene>& owner);

    std::shared_ptr<UiCanvas> CreateCanvas(const ViewPortInfo& canvasScreenSize, const std::string& name = std::string(""));
#ifdef DEBUG
    std::shared_ptr<UiCanvas> CreateDebugCanvas(const ViewPortInfo& canvasScreenSize);
#endif

    std::shared_ptr<UiCanvas> CreateHudCanvas(const ViewPortInfo& canvasScreenSize);

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override;

    std::shared_ptr<UiCanvas> GetCanvasByName(const std::string& canvasName) const;

    std::shared_ptr<UiCanvas> GetCanvasByUId(const uint32_t canvasId) const;

    bool CheckIfUiInterceptsMouseEvent(const glm::ivec2& currentMousePosition) const;

    void CleanUp();

    std::shared_ptr<UiCanvas> GetHudCanvas() const;

    std::shared_ptr<UiItemBase> GetUiItemByUId(const size_t uiItemUId) const;
};
} // namespace GUI
} // namespace EngineCore