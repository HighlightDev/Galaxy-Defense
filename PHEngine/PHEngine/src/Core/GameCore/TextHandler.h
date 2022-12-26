#pragma once

#include <memory>
#include <vector>

#include "Core/GameCore/GUI/Text/TextField.h"
#include "Core/GameCore/Event/TextEvent.h"

using namespace Event;

namespace EngineCore
{
    class Scene;

    class TextHandler
        : public TextRegisterEvent::Event_t,
          public TextDataChangedEvent::Event_t
    {

        std::vector<std::shared_ptr<TextField>> mRegisteredTexts;

    public:
        TextHandler();

        ~TextHandler();

        std::weak_ptr<Scene> mSceneWp;

        void SetScene(const std::weak_ptr<Scene> &sceneWp);

        std::shared_ptr<TextField> GetTextFieldById(const int32_t fieldId) const;

    protected:
        void ProcessEvent(const TextRegisterEvent::EventData_t &data) override;

        void ProcessEvent(const TextDataChangedEvent::EventData_t &data) override;
    };

}