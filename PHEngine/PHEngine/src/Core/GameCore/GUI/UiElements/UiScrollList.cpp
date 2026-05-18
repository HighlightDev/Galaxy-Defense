#include "UiScrollList.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiverBase.h"
#include "Core/GameCore/Input/MouseEventEnums.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/UiScrollListLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiScrollListSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

#include <algorithm>
#include <numeric>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Event;

namespace EngineCore {
namespace GUI {

class UiScrollListScrollHelper : public MouseScrollGameThreadEvent,
                                 public std::enable_shared_from_this<UiScrollListScrollHelper> {
    std::weak_ptr<UiScrollList> mOwnerWp;

public:
    explicit UiScrollListScrollHelper(const std::weak_ptr<UiScrollList>& ownerWp)
        : mOwnerWp(ownerWp)
    {
    }

    void ProcessEvent(const MouseScrollGameThreadEvent* /*sender*/, const MouseScrollGameThreadEvent::EventData_t& data) override
    {
        if (eMouseEventTargetReceiverType::UI_INPUT_SYSTEM != std::get<0>(data)) {
            return;
        }
        if (const auto& ownerSp = mOwnerWp.lock()) {
            ownerSp->HandleScroll(std::get<1>(data));
        }
    }
};

UiScrollList::UiScrollList(const std::string& name)
    : UiItem(name)
{
    mIsGuiScissorsMaster = true;
    mIsGuiScissorsSlave = false;
}

UiScrollList::~UiScrollList()
{
    if (mScrollHelper) {
        MouseScrollGameThreadEvent::GetInstance()->RemoveListener(mScrollHelper->GetInstanceId());
        mScrollHelper.reset();
    }
}

void UiScrollList::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiScrollList>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }

    mScrollHelper = std::make_shared<UiScrollListScrollHelper>(std::static_pointer_cast<UiScrollList>(shared_from_this()));
    MouseScrollGameThreadEvent::GetInstance()->AddListener(mScrollHelper);

    auto mouseReceiver = std::make_shared<UiMouseInputReceiverBase>(std::static_pointer_cast<UiScrollList>(shared_from_this()));
    mouseReceiver->SetMouseHoverEnteredCallback(
        [weakThis = std::weak_ptr<UiScrollList>(std::static_pointer_cast<UiScrollList>(shared_from_this()))](auto, auto) {
            if (const auto& sp = weakThis.lock()) {
                sp->mIsHovered = true;
            }
        });
    mouseReceiver->SetMouseHoverLeavedCallback(
        [weakThis = std::weak_ptr<UiScrollList>(std::static_pointer_cast<UiScrollList>(shared_from_this()))](auto, auto) {
            if (const auto& sp = weakThis.lock()) {
                sp->mIsHovered = false;
            }
        });
    SetMouseInputReceiver(mouseReceiver);
    SetIfCanInterceptMouseInputEvents(true);
}

void UiScrollList::OnUnregistered()
{
    if (mScrollHelper) {
        MouseScrollGameThreadEvent::GetInstance()->RemoveListener(mScrollHelper->GetInstanceId());
        mScrollHelper.reset();
    }
}

void UiScrollList::HandleScroll(const EngineCore::eMouseScrollDirection direction)
{
    if (!mIsHovered) {
        return;
    }
    if (direction == eMouseScrollDirection::ZoomOut) {
        SetScrollOffset(mScrollOffset + mScrollSpeed);
    } else if (direction == eMouseScrollDirection::ZoomIn) {
        SetScrollOffset(mScrollOffset - mScrollSpeed);
    }
}

void UiScrollList::SetScrollOffset(const int32_t offset)
{
    const int32_t clamped = std::clamp(offset, 0, GetMaxScrollOffset());
    if (mScrollOffset != clamped) {
        mScrollOffset = clamped;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsTransformDirty(true);
    }
}

int32_t UiScrollList::GetScrollOffset() const
{
    return mScrollOffset;
}

void UiScrollList::SetScrollSpeed(const int32_t speed)
{
    if (mScrollSpeed != speed) {
        mScrollSpeed = speed;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

int32_t UiScrollList::GetScrollSpeed() const
{
    return mScrollSpeed;
}

void UiScrollList::SetSpacing(const uint32_t spacing)
{
    if (mSpacing != spacing) {
        mSpacing = spacing;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsTransformDirty(true);
    }
}

uint32_t UiScrollList::GetSpacing() const
{
    return mSpacing;
}

void UiScrollList::SetScrollbarSide(const eScrollbarSide side)
{
    if (mScrollbarSide != side) {
        mScrollbarSide = side;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

eScrollbarSide UiScrollList::GetScrollbarSide() const
{
    return mScrollbarSide;
}

void UiScrollList::SetScrollbarBackgroundColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mScrollbarBackgroundColor)) {
        mScrollbarBackgroundColor = color;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

glm::vec3 UiScrollList::GetScrollbarBackgroundColor() const
{
    return mScrollbarBackgroundColor;
}

void UiScrollList::SetScrollbarThumbColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mScrollbarThumbColor)) {
        mScrollbarThumbColor = color;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

glm::vec3 UiScrollList::GetScrollbarThumbColor() const
{
    return mScrollbarThumbColor;
}

void UiScrollList::SetScrollbarThicknessPixels(const uint32_t thicknessPixels)
{
    if (mScrollbarThicknessPixels != thicknessPixels) {
        mScrollbarThicknessPixels = thicknessPixels;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

uint32_t UiScrollList::GetScrollbarThicknessPixels() const
{
    return mScrollbarThicknessPixels;
}

int32_t UiScrollList::GetMaxScrollOffset() const
{
    if (mHeight == 0) {
        return 0;
    }
    const uint32_t visibleCount
        = std::count_if(mChildren.cbegin(), mChildren.cend(), [](const auto& c) { return c->IsVisible(); });
    if (visibleCount == 0) {
        return 0;
    }
    const int32_t totalHeight
        = std::accumulate(mChildren.cbegin(), mChildren.cend(), 0, [](const int32_t sum, const std::shared_ptr<UiItemBase>& c) {
              return c->IsVisible() ? sum + static_cast<int32_t>(c->GetHeight()) : sum;
          });
    const int32_t spacingsTotal = static_cast<int32_t>((visibleCount - 1) * mSpacing);
    const int32_t contentHeight = totalHeight + spacingsTotal;
    return std::max(0, contentHeight - static_cast<int32_t>(mHeight));
}

void UiScrollList::RecalculatePositionsForChildren()
{
    const uint32_t visibleCount
        = std::count_if(mChildren.cbegin(), mChildren.cend(), [](const auto& c) { return c->IsVisible(); });
    if (visibleCount == 0 || mWidth == 0 || mHeight == 0) {
        return;
    }

    const auto childHeightZero = std::any_of(
        mChildren.cbegin(), mChildren.cend(), [](const auto& c) { return c->IsVisible() ? c->GetHeight() == 0 : false; });
    if (childHeightZero) {
        return;
    }

    // cursorY starts at the "virtual top" of the content area.
    // mAbsoluteOrigin is bottom-left; top of viewport = mAbsoluteOrigin.y + mHeight.
    // mScrollOffset shifts the content upward so lower items come into view.
    int32_t cursorY = static_cast<int32_t>(mAbsoluteOrigin.y + mHeight) + mScrollOffset;

    for (const auto& child : mChildren) {
        if (!child->IsVisible()) {
            continue;
        }
        const auto& anchors = child->GetAnchors();
        ext_assert(anchors.size() == 0, "Ui widget cannot have anchors inside UiScrollList.");

        cursorY -= static_cast<int32_t>(child->GetHeight());
        child->SetAbsoluteOrigin(glm::ivec2(mAbsoluteOrigin.x, cursorY));
        cursorY -= static_cast<int32_t>(mSpacing);
    }
}

void UiScrollList::UpdateAnchorTransform()
{
    UiItem::UpdateAnchorTransform();
    RecalculatePositionsForChildren();
    PropagateGuiScissorsToChildren();
}

void UiScrollList::UnpausableTick(const float deltaTimeSec)
{
    const bool childDirty = std::any_of(
        mChildren.cbegin(), mChildren.cend(), [](const auto& c) { return c->IsTransformDirty() || c->IsVisibleDirty(); });
    if (childDirty) {
        SetIsTransformDirty(true);
    }

    UiItem::UnpausableTick(deltaTimeSec);
}

void UiScrollList::RemoveUiItem(const std::shared_ptr<UiItemBase>& uiItem)
{
    uiItem->SetIsGuiScissorsSlaveRecursive(false);
    UiItem::RemoveUiItem(uiItem);
}

std::shared_ptr<Graphics::Proxy::UiSceneProxyBase> UiScrollList::CreateUiSceneProxy() const
{
    return std::make_shared<Graphics::Proxy::UiScrollListSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiScrollList::ReplicateLuaProxy()
{
    return std::make_shared<UiScrollListLuaProxy>(std::static_pointer_cast<UiScrollList>(shared_from_this()));
}

std::string UiScrollList::GetUiTypeString() const
{
    return "UiScrollList";
}

void UiScrollList::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItem::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("spacing")) {
        const auto spacing = static_cast<uint32_t>(nlohmann_utilities::GetIntFromJson(jsonObj, "spacing"));
        if (spacing != mSpacing) {
            mSpacing = spacing;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("scroll_offset")) {
        const auto offset = nlohmann_utilities::GetIntFromJson(jsonObj, "scroll_offset");
        SetScrollOffset(offset);
    }
    if (jsonObj.contains("scroll_speed")) {
        const auto speed = nlohmann_utilities::GetIntFromJson(jsonObj, "scroll_speed");
        if (speed != mScrollSpeed) {
            mScrollSpeed = speed;
        }
    }
    if (jsonObj.contains("scrollbar_side")) {
        const auto sideInt = nlohmann_utilities::GetIntFromJson(jsonObj, "scrollbar_side");
        const auto side = static_cast<eScrollbarSide>(sideInt);
        if (side != mScrollbarSide) {
            mScrollbarSide = side;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("scrollbar_background_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["scrollbar_background_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mScrollbarBackgroundColor)) {
            mScrollbarBackgroundColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("scrollbar_thumb_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["scrollbar_thumb_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mScrollbarThumbColor)) {
            mScrollbarThumbColor = color;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("scrollbar_thickness")) {
        const auto thickness = static_cast<uint32_t>(nlohmann_utilities::GetIntFromJson(jsonObj, "scrollbar_thickness"));
        if (thickness != mScrollbarThicknessPixels) {
            mScrollbarThicknessPixels = thickness;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
}

void UiScrollList::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();
    SyncDataOnLuaThread();
}

void UiScrollList::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    SyncDataOnRenderThread();
}

void UiScrollList::PropagateGuiScissorsToChildren()
{
    for (const auto& child : mChildren) {
        child->SetIsGuiScissorsSlaveRecursive(true);
    }
}

void UiScrollList::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiScrollList::SyncDataOnLuaThread");
    if (mIsLuaProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& luaScriptProcessorSp = GetLuaScriptProcessorWp().lock()) {
                SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetUId(),
                    functionId,
                    [luaScriptProcessorSp,
                     luaProxyId = GetLuaProxyId(),
                     spacing = mSpacing,
                     scrollOffset = mScrollOffset,
                     scrollSpeed = mScrollSpeed,
                     scrollbarSide = mScrollbarSide,
                     scrollbarBackgroundColor = mScrollbarBackgroundColor,
                     scrollbarThumbColor = mScrollbarThumbColor,
                     scrollbarThickness = mScrollbarThicknessPixels](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& proxy
                            = std::static_pointer_cast<UiScrollListLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            proxy->SetSpacing_FromGameThread(spacing);
                            proxy->SetScrollOffset_FromGameThread(scrollOffset);
                            proxy->SetScrollSpeed_FromGameThread(scrollSpeed);
                            proxy->SetScrollbarSide_FromGameThread(static_cast<uint8_t>(scrollbarSide));
                            proxy->SetScrollbarBackgroundColor_FromGameThread(scrollbarBackgroundColor);
                            proxy->SetScrollbarThumbColor_FromGameThread(scrollbarThumbColor);
                            proxy->SetScrollbarThicknessPixels_FromGameThread(scrollbarThickness);
                        }
                    });
            }
        }
    }
}

void UiScrollList::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiScrollList::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& canvasSp = GetParentCanvas().lock()) {
                if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetUId(),
                        functionId,
                        [sceneRenderer,
                         myUId = GetUId(),
                         canvasUId = canvasSp->GetUId(),
                         scrollOffset = mScrollOffset,
                         maxScrollOffset = GetMaxScrollOffset(),
                         scrollbarSide = static_cast<uint8_t>(mScrollbarSide),
                         scrollbarBackgroundColor = mScrollbarBackgroundColor,
                         scrollbarThumbColor = mScrollbarThumbColor,
                         scrollbarThickness = mScrollbarThicknessPixels](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& scrollListProxy
                                    = std::static_pointer_cast<Graphics::Proxy::UiScrollListSceneProxy>(uiSceneProxy);
                                scrollListProxy->SetScrollOffset(scrollOffset);
                                scrollListProxy->SetMaxScrollOffset(maxScrollOffset);
                                scrollListProxy->SetScrollbarSide(scrollbarSide);
                                scrollListProxy->SetScrollbarBackgroundColor(scrollbarBackgroundColor);
                                scrollListProxy->SetScrollbarThumbColor(scrollbarThumbColor);
                                scrollListProxy->SetScrollbarThicknessPixels(scrollbarThickness);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

} // namespace GUI
} // namespace EngineCore
