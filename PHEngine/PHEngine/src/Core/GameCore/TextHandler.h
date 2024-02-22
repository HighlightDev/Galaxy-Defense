#pragma once

#include <memory>
#include <vector>

#include "Core/GameCore/GUI/HudText/HudTextField.h"
#include "Core/GameCore/Event/TextEvent.h"

using namespace Event;

namespace EngineCore
{
    class Scene;

    class TextHandler
        : public TextRegisterGameThreadEvent,
          public TextDataChangedGameThreadEvent,
          public std::enable_shared_from_this<TextHandler>
    {
        std::vector<std::shared_ptr<HudTextField>> mRegisteredTexts;

    public:
        TextHandler();

        ~TextHandler();

        void Initialize();

        std::weak_ptr<Scene> mSceneWp;

        void SetScene(const std::weak_ptr<Scene> &sceneWp);

        std::shared_ptr<HudTextField> GetTextFieldById(const int32_t fieldId) const;

    protected:
        void ProcessEvent(const TextRegisterGameThreadEvent::EventData_t &data) override;

        void ProcessEvent(const TextDataChangedGameThreadEvent::EventData_t &data) override;
    };

}