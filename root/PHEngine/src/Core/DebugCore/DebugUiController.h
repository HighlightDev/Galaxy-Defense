#pragma once

#include "Core/GameCore/Event/WindowSizeChangedEvent.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/ResourceManagerCore/Pool/ITextureObtainable.h"

#include <memory>
#include <utility>
#include <vector>

using namespace Graphics::Texture;
using namespace Resources;
using namespace Event;

namespace EngineCore::GUI {
class UiCanvas;
class UiImage;
class UiItemBase;
class UiItem;
class UiLabel;
class UiRectangle;
class UiRowLayout;
} // namespace EngineCore::GUI

namespace EngineCore {

class Scene;
class UiInputComponent;

namespace Debug {
class DebugUiController : public ITickable,
                          public WindowSizeChangedGameThreadEvent,
                          public std::enable_shared_from_this<DebugUiController> {
    std::weak_ptr<::EngineCore::Scene> mSceneWp;

    std::shared_ptr<::EngineCore::GUI::UiCanvas> mCanvas;
    std::vector<std::pair<std::shared_ptr<::EngineCore::GUI::UiLabel>, std::shared_ptr<::EngineCore::GUI::UiImage>>> mImagePairs;

    std::unique_ptr<UiInputComponent> mInputComponent;

    mutable size_t mTextureIndex{0};

    float mPressButtonCooldown;

    size_t mPoolIndex{0};

    std::vector<std::shared_ptr<ITextureObtainable>> mPools;

    std::shared_ptr<::EngineCore::GUI::UiLabel> mRenderFpsLabel;
    std::shared_ptr<::EngineCore::GUI::UiLabel> mGameFpsLabel;
    std::shared_ptr<::EngineCore::GUI::UiLabel> mLuaFpsLabel;

    std::shared_ptr<::EngineCore::GUI::UiImage> mImage1;
    std::shared_ptr<::EngineCore::GUI::UiImage> mImage2;
    std::shared_ptr<::EngineCore::GUI::UiLabel> mImage1Label;
    std::shared_ptr<::EngineCore::GUI::UiLabel> mImage2Label;
    std::shared_ptr<::EngineCore::GUI::UiItem> mImage1Container;
    std::shared_ptr<::EngineCore::GUI::UiItem> mImage2Container;

    std::shared_ptr<::EngineCore::GUI::UiRowLayout> mTexturesLayout;
    std::shared_ptr<::EngineCore::GUI::UiRectangle> mRectangleBackground;
    std::shared_ptr<::EngineCore::GUI::UiImage> mNextPoolsArrowImage;

public:
    DebugUiController();

    ~DebugUiController();

    void Initialize();

    void SetScene(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    void PostPlayLevelFinished();

    void SetRenderFpsText(const std::string& fpsText);

    void SetGameFpsText(const std::string& fpsText);

    void SetLuaFpsText(const std::string& fpsText);

    void ProcessEvent(
        const WindowSizeChangedGameThreadEvent* sender, const WindowSizeChangedGameThreadEvent::EventData_t& data) override;

private:
    void InitializeWidgets();

    void GoToNextTexture();

    std::shared_ptr<ITexture> GetCurrentTexture() const;

    std::string GetCurrentTextureName() const;

    void OnNextPoolButtonClicked(const std::weak_ptr<UiItemBase>& senderWp, const glm::ivec2& mouseCursorPosition);

    void RecalculateWidgetsSize();
};
} // namespace Debug
} // namespace EngineCore