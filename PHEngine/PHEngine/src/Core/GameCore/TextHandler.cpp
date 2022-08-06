#include "TextHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>
#include <glm/vec3.hpp>

namespace EngineCore
{
    TextHandler::TextHandler()
        : mSceneWp(),
          mRegisteredTexts()
    {
        TextRegisterEvent::GetInstance()->AddListener(this);
        TextDataChangedEvent::GetInstance()->AddListener(this);
    }

    TextHandler::~TextHandler()
    {
        TextRegisterEvent::GetInstance()->RemoveListener(this);
        TextDataChangedEvent::GetInstance()->RemoveListener(this);
    }

    void TextHandler::SetScene(const std::weak_ptr<Scene> &sceneWp)
    {
        mSceneWp = sceneWp;
    }

    std::shared_ptr<TextField> TextHandler::GetTextFieldById(const int32_t fieldId) const
    {
        auto foundIt = std::find_if(mRegisteredTexts.begin(), mRegisteredTexts.end(), [=](const auto &textField)
                                    { return fieldId == textField->GetTextFieldId(); });

        return foundIt != mRegisteredTexts.end() ? *foundIt : nullptr;
    }

    void TextHandler::ProcessEvent(const TextRegisterEvent::EventData_t &data)
    {
        if (auto sceneSp = mSceneWp.lock())
        {
            const auto &textSp = std::get<0>(data);
            const auto registerType = std::get<1>(data);
            const bool bSubscribeOnUpdateTextScreenSpaceSize = std::get<2>(data);

            if (eRegisterType::REGISTER == registerType)
            {
                mRegisteredTexts.emplace_back(textSp);
                sceneSp->RegisterText_OnRenderThread(textSp, bSubscribeOnUpdateTextScreenSpaceSize);
            }
            else
            {
                auto removeIt = std::remove_if(mRegisteredTexts.begin(), mRegisteredTexts.end(), [=](const auto &textField)
                                               { return textSp->GetTextFieldId() == textField->GetTextFieldId(); });

                if (removeIt != mRegisteredTexts.end())
                {
                    mRegisteredTexts.erase(removeIt);
                }
                sceneSp->UnregisterText_OnRenderThread(textSp);
            }
        }
    }

    void TextHandler::ProcessEvent(const TextDataChangedEvent::EventData_t &data)
    {
        if (auto sceneSp = mSceneWp.lock())
        {
            const auto &textSp = std::get<0>(data);
            const auto &changedDataType = std::get<1>(data);
            sceneSp->TextDataChanged_OnRenderThread(textSp, changedDataType);
        }
    }
}