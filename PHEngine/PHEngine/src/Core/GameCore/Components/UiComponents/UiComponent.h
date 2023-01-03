#pragma once

#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/GUI/HudText/HudTextField.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"

namespace EngineCore
{
    struct ComponentData;

    class UiComponent
        : public Component
    {
    protected:
        std::vector<std::shared_ptr<HudTextField>> mTextFields;

    public:
        UiComponent(const ComponentData &data);

        ~UiComponent() override;

        void Tick(const float deltaTime) override;

        void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

        eComponentType GetComponentType() const override;

        int32_t CreateTextField(const std::string &fontName,
                               const float fontSize,
                               const std::string &text,
                               const glm::vec3 &color,
                               const glm::vec2 &position,
                               const bool receiveUpdateOnTextScreenSpaceSizeChanged,
                               const float lineMaxSize,
                               const int32_t numberOfLines,
                               const eTextHorizontalAlignmentType textHorizontalAlignment);

        int32_t CreateEmptyTextField(const std::string &fontName,
                                    const float fontSize,
                                    const glm::vec3 &color,
                                    const bool receiveUpdateOnTextScreenSpaceSizeChanged,
                                    const float lineMaxSize,
                                    const int32_t numberOfLines,
                                    const eTextHorizontalAlignmentType textHorizontalAlignment);

        void DeleteTextField(const int32_t textFieldId);

        std::weak_ptr<HudTextField> GetTextFieldById(const int32_t textFieldId) const;

        const std::vector<std::shared_ptr<HudTextField>> &GetTextFields() const;
    };
}