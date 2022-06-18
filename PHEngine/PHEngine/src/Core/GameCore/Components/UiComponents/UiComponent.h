#pragma once

#include <vector>

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/GUI/Text/TextField.h"

namespace EngineCore
{
    class UiComponent
        : public Component
    {
        protected:

        std::vector<std::shared_ptr<TextField>> mTextFields;

    public:
        virtual void Tick(const float deltaTime) override;

        virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

        virtual eComponentType GetComponentType() const override;
    };
}