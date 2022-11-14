#include "UiImage.h"

namespace EngineCore
{
    namespace GUI
    {
        UiImage::UiImage(const std::weak_ptr<UiItemBase> &parent)
            : UiItemBase(parent)
        {
        }
        void UiImage::Render()
        {
        }

        void UiImage::UpdateHierarchyTransform()
        {
        }
    }
}