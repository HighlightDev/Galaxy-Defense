#include "TextHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>
#include <glm/vec3.hpp>

namespace EngineCore
{
    TextHandler::TextHandler()
        : mSceneWp()
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

    void TextHandler::ProcessEvent(const TextRegisterEvent::EventData_t &data)
    {
        if (auto sceneSp = mSceneWp.lock())
        {
            const auto &textSp = std::get<0>(data);
            const auto registerType = std::get<1>(data);

            if (eRegisterType::REGISTER == registerType)
            {
                sceneSp->RegisterText_OnRenderThread(textSp);
            }
            else
            {
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