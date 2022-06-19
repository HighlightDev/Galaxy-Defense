#pragma once

#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/GUI/Text/TextField.h"

namespace EngineCore
{
    struct ComponentData;

    class UiComponent
        : public Component
    {
    protected:
        std::vector<std::shared_ptr<TextField>> mTextFields;

    public:
        UiComponent(const ComponentData& data);

        virtual ~UiComponent() override;

        virtual void Tick(const float deltaTime) override;

        virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

        virtual eComponentType GetComponentType() const override;

        size_t CreateTextField(const std::string &fontName,
                               const float fontSize,
                               const std::string &text,
                               const glm::vec3 &color,
                               const glm::vec2 &position,
                               const float lineMaxSize,
                               const int32_t numberOfLines,
                               const bool isCenteredText);

        void DeleteTextField(const size_t textFieldId);

        std::shared_ptr<TextField> GetTextFieldById(const size_t textFieldId) const;

        const std::vector<std::shared_ptr<TextField>>& GetTextFields() const;
    };
}