#include "UiCanvasSceneProxy.h"

#include "Core/CommonCore/EngineConstants.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontHandler.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GraphicsCore/Renderer/RenderState.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Core/UtilityCore/EngineMath.h"

#include <algorithm>

using namespace EngineCore::GUI;
using namespace EngineCore;

namespace Graphics {
namespace Proxy {
UiCanvasSceneProxy::UiCanvasSceneProxy(const UiCanvas* canvas)
    : mUiItemUId(canvas->GetUId())
    , mIsVisible(canvas->IsVisible())
    , mAbsoluteOrigin(canvas->GetAbsoluteOrigin())
    , mWidthHeight(glm::ivec2(canvas->GetWidth(), canvas->GetHeight()))
    , mFontHandlerWp()
    , mOverlayOpacity(1.0f)
    , mCanvasZOrder(canvas->GetZOrder())
    , mName(canvas->GetName())
{
}

void UiCanvasSceneProxy::CleanUp()
{
    for (const auto& uiProxy : mUiProxies) {
        uiProxy->CleanUp();
    }
    mUiProxies.clear();
}

void UiCanvasSceneProxy::AddUiSceneProxy(std::shared_ptr<UiSceneProxyBase> uiProxy)
{
    mUiProxies.emplace_back(uiProxy);
}

void UiCanvasSceneProxy::RemoveUiSceneProxy(const size_t uiItemUId)
{
    mUiProxies.erase(std::remove_if(
        mUiProxies.begin(), mUiProxies.end(), [uiItemUId](const auto& proxy) { return uiItemUId == proxy->GetUiItemUId(); }));
}

void UiCanvasSceneProxy::SortProxiesByZOrder()
{
    // Lexicographical comparison of z-path: parent path is a prefix of child path, so parent is always sorted before. stable_sort
    // preserves registration order for completely equal paths.
    std::stable_sort(mUiProxies.begin(), mUiProxies.end(), [](const auto& left, const auto& right) {
        return left->GetZPath() < right->GetZPath();
    });
}

void UiCanvasSceneProxy::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy)
{
    RenderState renderState;
    if (mIsVisible && !EngineMath::FloatsNearEqual(mOverlayOpacity, 0.0f)) {
        for (const auto& proxy : mUiProxies) {
            if (proxy->IsVisible()) {
                proxy->SetOverlayOpacity(mOverlayOpacity);
                const GLuint bloomRefValue = proxy->CanBloomBeApplied() ? EngineConstants::eStencilValues::BLOOM : 0;
                if (proxy->IsGuiScissorsSlave()) {
                    const GLint ref = EngineConstants::eStencilValues::GUI_SCISSORING | bloomRefValue;
                    // test: compare ONLY the GUI_SCISSORING bit — slave is rendered inside the scissor region
                    // the master is independent of whether the master wrote the BLOOM bit or not.
                    // Write: write-mask = bloomRefValue. If slave has bloom — only the BLOOM bit is passed
                    // BLOOM (GL_REPLACE will set it from ref), GUI_SCISSORING master is not touched.
                    // If slave has no bloom — mask 0, stencil is not changed at all.
                    renderState.GetStencilState()
                        .SetStencilFunction(GL_EQUAL, ref, EngineConstants::eStencilValues::GUI_SCISSORING)
                        .SetStencilMask(bloomRefValue);
                    renderState.BindRenderState();
                } else {
                    // Master and usual proxies write the full value of ref.
                    GLint ref, mask;
                    if (proxy->IsGuiScissorsMaster()) {
                        ref = EngineConstants::eStencilValues::GUI_SCISSORING | bloomRefValue;
                        mask = 0xFF;
                    } else {
                        ref = 0x00;
                        mask = 0xFF;
                    }
                    renderState.GetStencilState().SetStencilMask(0xFF).SetStencilFunction(GL_ALWAYS, ref, mask);
                    renderState.BindRenderState();
                }
                proxy->Render(postFxRenderTargetProvider, cameraSceneProxy);
            }
        }
        renderState.GetStencilState().SetStencilMask(0xFF);
        renderState.BindRenderState();
    }
}

void UiCanvasSceneProxy::SetUiItemUid(const size_t UId)
{
    mUiItemUId = UId;
}

size_t UiCanvasSceneProxy::GetUiItemUId() const
{
    return mUiItemUId;
}

void UiCanvasSceneProxy::SetCanvasZOrder(const size_t zOrder)
{
    mCanvasZOrder = zOrder;
}

size_t UiCanvasSceneProxy::GetCanvasZOrder() const
{
    return mCanvasZOrder;
}

void UiCanvasSceneProxy::SetIsVisible(const bool isVisible)
{
    if (mIsVisible != isVisible) {
        mIsVisible = isVisible;
    }
}

bool UiCanvasSceneProxy::IsVisible() const
{
    return mIsVisible;
}

void UiCanvasSceneProxy::SetAbsoluteOrigin(const glm::ivec2& position)
{
    mAbsoluteOrigin = position;
}

void UiCanvasSceneProxy::SetWidthHeight(const glm::ivec2& widthHeight)
{
    mWidthHeight = widthHeight;
}

void UiCanvasSceneProxy::SetOverlayOpacity(const float opacity)
{
    mOverlayOpacity = opacity;
}

float UiCanvasSceneProxy::GetOverlayOpacity() const
{
    return mOverlayOpacity;
}

glm::ivec2 UiCanvasSceneProxy::GetAbsoluteOrigin() const
{
    return mAbsoluteOrigin;
}

glm::ivec2 UiCanvasSceneProxy::GetWidthHeight() const
{
    return mWidthHeight;
}

std::shared_ptr<UiSceneProxyBase> UiCanvasSceneProxy::GetSceneProxyById(const size_t uid) const
{
    for (const auto proxy : mUiProxies) {
        if (uid == proxy->GetUiItemUId())
            return proxy;
    }
    return nullptr;
}

void UiCanvasSceneProxy::SetFontHandler(const std::weak_ptr<::EngineCore::GUI::FreeTypeFontHandler>& fontHandlerWp)
{
    mFontHandlerWp = fontHandlerWp;
}

std::weak_ptr<::EngineCore::GUI::FreeTypeFontHandler> UiCanvasSceneProxy::GetFontHandler() const
{
    return mFontHandlerWp;
}
} // namespace Proxy
} // namespace Graphics