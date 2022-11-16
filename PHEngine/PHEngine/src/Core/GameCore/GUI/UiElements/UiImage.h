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

        public:
            explicit UiImage(const std::weak_ptr<IUiTransformable> &parent = std::weak_ptr<IUiTransformable>());

            ~UiImage() override = default;

            virtual void Render() override;

        protected:
            virtual void UpdateHierarchyTransform() override;
        };
    }
}