#pragma once

#include <memory>
#include <vector>

#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/ResourceManagerCore/Pool/ITextureObtainable.h"

using namespace EngineCore::GUI;
using namespace Graphics::Texture;
using namespace Resources;

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

            mutable size_t mTextureIndex{0};

            float mPressButtonCooldown;

            size_t mPoolIndex{0};

            std::vector<std::shared_ptr<ITextureObtainable>> mPools;

        public:
            DebugUiController();

            void SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

            void Tick(const float deltaTime) override;

            void UnpausableTick(const float deltaTime) override;

            void PostPlayLevelFinished();

        private:
            void Init();

            std::shared_ptr<ITexture> GetNextTexture() const;

            void OnNextPoolButtonClicked(const std::weak_ptr<UiItemBase>& senderWp, const glm::ivec2 &mouseCursorPosition);
        };
    }
}