#include "UiSceneProxyBase.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"

using namespace EngineCore::GUI;
using namespace EngineCore::DataProviders;

namespace Graphics {
namespace Proxy {
UiSceneProxyBase::UiSceneProxyBase(const UiItemBase* uiItemBase)
    : mUiItemUId(uiItemBase->GetUId())
    , mIsVisible(false)
    , mZOrder(uiItemBase->GetZOrder())
    , mParentCanvasProxy()
    , mNormalizedTranslation(uiItemBase->GetNormalizedTranslation())
    , mNormalizedScale(uiItemBase->GetNormalizedScale())
    , mWidthHeightPixels(glm::ivec2(static_cast<int32_t>(uiItemBase->GetWidth()), static_cast<int32_t>(uiItemBase->GetWidth())))
    , mCenterOffset(glm::vec2(0.0f, 0.0f))
{
}

void UiSceneProxyBase::SetUiItemUid(const size_t UId)
{
    mUiItemUId = UId;
}

size_t UiSceneProxyBase::GetUiItemUId() const
{
    return mUiItemUId;
}

void UiSceneProxyBase::SetIsVisible(const bool isVisible)
{
    mIsVisible = isVisible;
}

bool UiSceneProxyBase::IsVisible() const
{
    return mIsVisible;
}

void UiSceneProxyBase::SetZOrder(const size_t zOrder)
{
    if (const auto& canvasSp = mParentCanvasProxy.lock()) {
        mZOrder = zOrder;
        canvasSp->SortProxiesByZOrder();
    }
}

size_t UiSceneProxyBase::GetZOrder() const
{
    return mZOrder;
}

void UiSceneProxyBase::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
{
    mNormalizedTranslation = normalizedTranslation;
    mNormalizedScale = normalizedScale;
}

void UiSceneProxyBase::SetWidthHeightPixels(const glm::ivec2& widthHeight)
{
    mWidthHeightPixels = widthHeight;
}

void UiSceneProxyBase::SetOverlayOpacity(const float overlayOpacity)
{
    mOverlayOpacity = overlayOpacity;
}

float UiSceneProxyBase::GetOverlayOpacity() const
{
    return mOverlayOpacity;
}

void UiSceneProxyBase::SetScale(const float scale)
{
    mScale = scale;
}

float UiSceneProxyBase::GetScale() const
{
    return mScale;
}

void UiSceneProxyBase::SetCenterOffset(const glm::vec2& offset)
{
    mCenterOffset = offset;
}

glm::vec2 UiSceneProxyBase::GetCenterOffset() const
{
    return mCenterOffset;
}

void UiSceneProxyBase::SetCanvasSceneProxy(const std::weak_ptr<UiCanvasSceneProxy>& parentCanvasProxy)
{
    mParentCanvasProxy = parentCanvasProxy;
}

void UiSceneProxyBase::Render()
{
}

glm::vec2 UiSceneProxyBase::GetNormalizedWidthHeight() const
{
    const auto& screenResolution = glm::ivec2(
        GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth(),
        GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight());
    return glm::vec2(
        static_cast<float>(mWidthHeightPixels.x) / static_cast<float>(screenResolution.x),
        static_cast<float>(mWidthHeightPixels.y) / static_cast<float>(screenResolution.y));
}

} // namespace Proxy
} // namespace Graphics