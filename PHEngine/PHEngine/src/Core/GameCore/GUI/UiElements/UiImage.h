#pragma once

#include "UiItemBase.h"
#include "Core/GameCore/ShaderImplementation/UiTestShader.h"

using namespace EngineCore::ShaderImpl;

namespace EngineCore
{
    namespace GUI
    {
        class UiImage : public UiItemBase
        {
            std::shared_ptr<UiTestShader> mUiTestShader;

            glm::vec4 mColor;

        public:
            explicit UiImage(const std::weak_ptr<IUiTransformable> &parent = std::weak_ptr<IUiTransformable>());

            ~UiImage() override = default;

            virtual void Render() override;

            void SetColor(const glm::vec4& color);

            glm::vec4 GetColor() const;

        protected:
            virtual void UpdateHierarchyTransform() override;
        };
    }
}