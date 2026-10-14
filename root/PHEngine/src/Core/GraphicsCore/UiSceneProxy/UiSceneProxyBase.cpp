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
    , mIsVisible(uiItemBase->IsVisible())
    , mZPath(uiItemBase->GetZPath())
    , mParentCanvasProxy()
    , mNormalizedTranslation(uiItemBase->GetNormalizedTranslation())
    , mNormalizedScale(uiItemBase->GetNormalizedScale())
    , mWidthHeightPixels(glm::ivec2(static_cast<int32_t>(uiItemBase->GetWidth()), static_cast<int32_t>(uiItemBase->GetHeight())))
    , mScale(1.0f)
    , mCenterOffset(0.0f)
    , mName(uiItemBase->GetName())
    , mIsGuiScissorsSlave(uiItemBase->IsGuiScissorsSlave())
    , mIsGuiScissorsMaster(uiItemBase->IsGuiScissorsMaster())
    , mCanBloomBeApplied(uiItemBase->CanBloomBeApplied())
{
}

void UiSceneProxyBase::SetSceneRenderer(const std::weak_ptr<Graphics::Renderer::SceneRenderer>& sceneRendererWp)
{
    mSceneRendererWp = sceneRendererWp;
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

void UiSceneProxyBase::SetZPath(const std::vector<int32_t>& zPath)
{
    if (const auto& canvasSp = mParentCanvasProxy.lock()) {
        mZPath = zPath;
        canvasSp->SortProxiesByZOrder();
    }
}

const std::vector<int32_t>& UiSceneProxyBase::GetZPath() const
{
    return mZPath;
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

void UiSceneProxyBase::SetScale(const glm::vec2& scale)
{
    mScale = scale;
}

glm::vec2 UiSceneProxyBase::GetScale() const
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

void UiSceneProxyBase::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy)
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

void UiSceneProxyBase::SetIsGuiScissorsSlave(const bool isScissorsSlave)
{
    mIsGuiScissorsSlave = isScissorsSlave;
}

bool UiSceneProxyBase::IsGuiScissorsSlave() const
{
    return mIsGuiScissorsSlave;
}

void UiSceneProxyBase::SetIsGuiScissorsMaster(const bool isScissorsMaster)
{
    mIsGuiScissorsMaster = isScissorsMaster;
}

bool UiSceneProxyBase::IsGuiScissorsMaster() const
{
    return mIsGuiScissorsMaster;
}

bool UiSceneProxyBase::CanBloomBeApplied() const
{
    return mCanBloomBeApplied;
}

void UiSceneProxyBase::SetCanBloomBeApplied(const bool canBloomBeApplied)
{
    mCanBloomBeApplied = canBloomBeApplied;
}

} // namespace Proxy
} // namespace Graphics