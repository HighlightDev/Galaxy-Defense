#include "TextHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

#include <algorithm>
#include <glm/vec3.hpp>

using namespace Graphics::Renderer;

namespace EngineCore
{
    TextHandler::TextHandler()
        : mSceneWp(),
          mRegisteredTexts()
    {
    }

    TextHandler::~TextHandler()
    {
        TextRegisterGameThreadEvent::GetInstance()->RemoveListener(TextRegisterGameThreadEvent::GetInstanceId());
        TextDataChangedGameThreadEvent::GetInstance()->RemoveListener(TextDataChangedGameThreadEvent::GetInstanceId());
    }

    void TextHandler::Initialize()
    {
        TextRegisterGameThreadEvent::GetInstance()->AddListener(shared_from_this());
        TextDataChangedGameThreadEvent::GetInstance()->AddListener(shared_from_this());
    }

    void TextHandler::SetScene(const std::weak_ptr<Scene> &sceneWp)
    {
        mSceneWp = sceneWp;
    }

    std::shared_ptr<HudTextField> TextHandler::GetTextFieldById(const int32_t fieldId) const
    {
        auto foundIt = std::find_if(mRegisteredTexts.begin(), mRegisteredTexts.end(), [=](const auto &textField)
                                    { return fieldId == textField->GetTextFieldId(); });

        return foundIt != mRegisteredTexts.end() ? *foundIt : nullptr;
    }

    void TextHandler::ProcessEvent(const TextRegisterGameThreadEvent::EventData_t &data)
    {
        if (const auto &sceneSp = mSceneWp.lock())
        {
            if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
                const auto &textSp = std::get<0>(data);
                const auto registerType = std::get<1>(data);
                const bool bSubscribeOnUpdateTextScreenSpaceSize = std::get<2>(data);

                if (eRegisterType::REGISTER == registerType)
                {
                    mRegisteredTexts.emplace_back(textSp);
                    sceneRendererSp->RegisterText_OnRenderThread(textSp, bSubscribeOnUpdateTextScreenSpaceSize);
                }
                else
                {
                    mRegisteredTexts.erase(std::remove_if(mRegisteredTexts.begin(), mRegisteredTexts.end(), [=](const auto &textField)
                                                          { return textSp->GetTextFieldId() == textField->GetTextFieldId(); }));
                    sceneRendererSp->UnregisterText_OnRenderThread(textSp);
                }
            }
        }
    }

    void TextHandler::ProcessEvent(const TextDataChangedGameThreadEvent::EventData_t &data)
    {
        if (auto sceneSp = mSceneWp.lock())
        {
            if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
                const auto &textWp = std::get<0>(data);
                if (const auto &textSp = textWp.lock())
                {
                    const auto &changedDataType = std::get<1>(data);
                    sceneRendererSp->TextDataChanged_OnRenderThread(textSp, changedDataType);
                }
            }
        }
    }
}