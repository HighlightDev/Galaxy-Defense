#pragma once

#include <memory>
#include <vector>

#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/ResourceManagerCore/Pool/ITextureObtainable.h"
#include "Core/GameCore/Event/WindowSizeChangedEvent.h"

using namespace EngineCore::GUI;
using namespace Graphics::Texture;
using namespace Resources;
using namespace Event;

namespace EngineCore
{
    class Scene;
    class UiInputComponent;

    namespace Debug
    {
        class DebugUiController
            : public ITickable,
              public WindowSizeChangedGameThreadEvent,
              public std::enable_shared_from_this<DebugUiController>
        {
            std::weak_ptr<::EngineCore::Scene> mSceneWp;

            std::shared_ptr<::EngineCore::GUI::UiCanvas> mCanvas;
            std::vector<std::shared_ptr<::EngineCore::GUI::UiImage>> mImages;

            std::unique_ptr<UiInputComponent> mInputComponent;

            mutable size_t mTextureIndex{0};

            float mPressButtonCooldown;

            size_t mPoolIndex{0};

            std::vector<std::shared_ptr<ITextureObtainable>> mPools;

            std::shared_ptr<::EngineCore::GUI::UiLabel> mRenderFpsLabel;
            std::shared_ptr<::EngineCore::GUI::UiLabel> mGameFpsLabel;
            std::shared_ptr<::EngineCore::GUI::UiLabel> mLuaFpsLabel;

            std::shared_ptr<::EngineCore::GUI::UiRectangle> mRectangleBackground;
            std::shared_ptr<::EngineCore::GUI::UiImage> mImage;
            std::shared_ptr<::EngineCore::GUI::UiImage> mImage1;
            std::shared_ptr<::EngineCore::GUI::UiImage> mNextPoolsArrowImage;

        public:
            DebugUiController();

            ~DebugUiController();

            void Initialize();

            void SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

            void Tick(const float deltaTime) override;

            void UnpausableTick(const float deltaTime) override;

            void PostPlayLevelFinished();

            void SetRenderFpsText(const std::string &fpsText);

            void SetGameFpsText(const std::string &fpsText);

            void SetLuaFpsText(const std::string &fpsText);

            void ProcessEvent(const WindowSizeChangedGameThreadEvent::EventData_t &data) override;

        private:
            void InitializeWidgets();

            std::shared_ptr<ITexture> GetNextTexture() const;

            void OnNextPoolButtonClicked(const std::weak_ptr<UiItemBase> &senderWp, const glm::ivec2 &mouseCursorPosition);

            void RecalculateWidgetsSize();
        };
    }
}