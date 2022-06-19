#pragma once

#include <memory>

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
    public:
        TextHandler();

        ~TextHandler();

        std::weak_ptr<Scene> mSceneWp;

        void SetScene(const std::weak_ptr<Scene> &sceneWp);

    protected:
        virtual void ProcessEvent(const TextRegisterEvent::EventData_t &data) override;

        virtual void ProcessEvent(const TextDataChangedEvent::EventData_t &data) override;
    };

}