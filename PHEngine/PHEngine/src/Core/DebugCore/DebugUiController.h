#pragma once

#include <memory>

#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace EngineCore::GUI;
using namespace Graphics::Texture;

namespace EngineCore
{
    class Scene;
    class InputComponent;

    namespace Debug
    {
        class DebugUiController
            : public ITickable
        {
            std::weak_ptr<::EngineCore::Scene> mSceneWp;

            std::shared_ptr<::EngineCore::GUI::UiCanvas> mCanvas;
            std::vector<std::shared_ptr<::EngineCore::GUI::UiImage>> mImages;

            std::unique_ptr<InputComponent> mInputComponent;

            mutable size_t mRenderTargetIndex{0};

            float mPressButtonCooldown;

        public:
            DebugUiController();

            void SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

            virtual void Tick(const float deltaTime) override;

            void PostPlayLevelFinished();

        private:
            void Init();

            std::shared_ptr<ITexture> GetNextRenderTargetTexture() const;
        };
    }
}