#include "UiCanvas.h"

namespace EngineCore
{
    namespace GUI
    {
        UiCanvas::UiCanvas(const ViewPortInfo &canvasScreenProperties)
            : mAbsoluteOrigin(glm::ivec2(canvasScreenProperties.OriginX, canvasScreenProperties.OriginY)),
              mRelativeOrigin(mAbsoluteOrigin),
              mWidthHeight(glm::ivec2(canvasScreenProperties.Width, canvasScreenProperties.Height)),
              mChildren()
        {
        }

        const Transform2D &UiCanvas::GetAbsoluteOrigin() const
        {
            return mAbsoluteOrigin;
        }

        const Transform2D &UiCanvas::GetRelativeOrigin() const
        {
            return mRelativeOrigin;
        }

        size_t UiCanvas::GetZOrder() const
        {
            return 0;
        }

        size_t UiCanvas::GetWidth() const
        {
            return mWidthHeight.x;
        }

        size_t UiCanvas::GetHeight() const
        {
            return mWidthHeight.y;
        }

        void UiCanvas::SetAbsoluteOrigin(const Transform2D &transform)
        {
            mAbsoluteOrigin = transform;
        }

        void UiCanvas::SetRelativeOrigin(const Transform2D &transform)
        {
        }

        void UiCanvas::SetZOrder(const size_t z_order)
        {
        }

        void UiCanvas::SetWidth(const size_t width)
        {
            mWidthHeight.x = width;
        }

        void UiCanvas::SetHeight(const size_t height)
        {
            mWidthHeight.y = height;
        }
    }
}