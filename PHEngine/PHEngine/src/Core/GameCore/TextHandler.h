#pragma once

#include <vector>
#include <memory>

#include "Core/GameCore/GUI/Text/TextField.h"
#include "Core/GameCore/Event/TextEvent.h"

using namespace Event;

namespace EngineCore
{

    class TextHandler : public TextRegisterEvent::Event_t
    {
    public:

        TextHandler();

        ~TextHandler();

        std::vector<std::shared_ptr<TextField>> mTextFields;

    protected:
        virtual void ProcessEvent(const TextRegisterEvent::EventData_t &data) override;
    };

}