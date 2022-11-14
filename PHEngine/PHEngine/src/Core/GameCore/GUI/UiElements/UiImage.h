#pragma once

#include "UiItemBase.h"

namespace EngineCore
{
    namespace GUI
    {
        class UiImage : public UiItemBase
        {
        public:
            explicit UiImage(const std::weak_ptr<UiItemBase> &parent = std::weak_ptr<UiItemBase>());

            ~UiImage() override = default;

            void Render();

        protected:
            virtual void UpdateHierarchyTransform() override;
        };
    }
}