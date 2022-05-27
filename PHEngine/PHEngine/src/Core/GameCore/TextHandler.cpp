#include "TextHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

namespace EngineCore
{
    TextHandler::TextHandler()
        : mSceneWp(),
          mTextFields()
    {
        TextRegisterEvent::GetInstance()->AddListener(this);
    }

    TextHandler::~TextHandler()
    {
        TextRegisterEvent::GetInstance()->RemoveListener(this);
    }

    void TextHandler::SetScene(const std::weak_ptr<Scene> &sceneWp)
    {
        mSceneWp = sceneWp;
    }

    void TextHandler::ProcessEvent(const TextRegisterEvent::EventData_t &data)
    {
        const auto &textSp = std::get<0>(data);
        const auto registerType = std::get<1>(data);

        const auto it = std::find_if(mTextFields.begin(),
                                     mTextFields.end(),
                                     [textSp](const auto &textFieldSp)
                                     { return textFieldSp->GetTextFieldId() == textSp->GetTextFieldId(); });

        if (eRegisterType::REGISTER == registerType)
        {
            assert(mTextFields.end() == it);
            mTextFields.emplace_back(textSp);
        }
        else
        {
            assert(mTextFields.end() != it);
            mTextFields.erase(it);
        }
    }
}