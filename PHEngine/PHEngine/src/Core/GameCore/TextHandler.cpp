#include "TextHandler.h"

namespace EngineCore
{
    TextHandler::TextHandler()
        : mTextFields()
    {
        TextRegisterEvent::GetInstance()->AddListener(this);
    }

    TextHandler::~TextHandler()
    {
        TextRegisterEvent::GetInstance()->RemoveListener(this);
    }

    void TextHandler::ProcessEvent(const TextRegisterEvent::EventData_t &data)
    {
        const auto textPtr = std::get<0>(data);
        const auto registerType = std::get<1>(data);
    }
}