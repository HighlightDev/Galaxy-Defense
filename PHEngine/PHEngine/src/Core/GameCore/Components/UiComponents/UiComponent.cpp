#include "UiComponent.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <algorithm>

namespace EngineCore
{
    UiComponent::UiComponent(const ComponentData &data)
        : Component(data.GameObjectName),
          mTextFields()
    {
    }

    UiComponent::~UiComponent()
    {
        std::for_each(mTextFields.begin(), mTextFields.end(), [](const auto &textFieldSp)
                      { textFieldSp->UnregisterText(); });
    }

    void UiComponent::Tick(const float deltaTime)
    {
    }

    void UiComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
    {
    }

    eComponentType UiComponent::GetComponentType() const
    {
        return UI_COMPONENT;
    }

    int32_t UiComponent::CreateTextField(const std::string &fontName,
                                        const float fontSize,
                                        const std::string &text,
                                        const glm::vec3 &color,
                                        const glm::vec2 &position,
                                        const bool receiveUpdateOnTextScreenSpaceSizeChanged,
                                        const float lineMaxSize,
                                        const int32_t numberOfLines,
                                        const bool isCenteredText)
    {
        const auto &textFieldSp = std::make_shared<TextField>(fontName, fontSize, text, color, position, lineMaxSize, numberOfLines, isCenteredText);
        mTextFields.emplace_back(textFieldSp);
        textFieldSp->RegisterText(receiveUpdateOnTextScreenSpaceSizeChanged);
        return textFieldSp->GetTextFieldId();
    }

    int32_t UiComponent::CreateEmptyTextField(const std::string &fontName,
                                             const float fontSize,
                                             const glm::vec3 &color,
                                             const bool receiveUpdateOnTextScreenSpaceSizeChanged,
                                             const float lineMaxSize,
                                             const int32_t numberOfLines,
                                             const bool isCenteredText)
    {
        const auto &textFieldSp = std::make_shared<TextField>(fontName, fontSize, color, lineMaxSize, numberOfLines, isCenteredText);
        mTextFields.emplace_back(textFieldSp);
        textFieldSp->RegisterText(receiveUpdateOnTextScreenSpaceSizeChanged);
        return textFieldSp->GetTextFieldId();
    }

    void UiComponent::DeleteTextField(const int32_t textFieldId)
    {
        const auto it = std::find_if(
            mTextFields.begin(), mTextFields.end(), [=](const auto &textFieldSp)
            { return textFieldSp->GetTextFieldId() == textFieldId; });
        assert(it != mTextFields.end());
        (*it)->UnregisterText();
        mTextFields.erase(it);
    }

    std::weak_ptr<TextField> UiComponent::GetTextFieldById(const int32_t textFieldId) const
    {
        const auto it = std::find_if(
            mTextFields.begin(), mTextFields.end(), [=](const auto &textFieldSp)
            { return textFieldSp->GetTextFieldId() == textFieldId; });
        assert(it != mTextFields.end());
        return *it;
    }

    const std::vector<std::shared_ptr<TextField>> &UiComponent::GetTextFields() const
    {
        return mTextFields;
    }
}